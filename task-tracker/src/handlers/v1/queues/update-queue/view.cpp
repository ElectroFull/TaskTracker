#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../lib/auth.hpp"
#include "../../../../helpers/helpers.hpp"
#include "../../../../models/queue.hpp"
#include "../../../../queries/queues_queries.hpp"

namespace tracker {

namespace {

class UpdateQueue final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-update-queue";

    UpdateQueue(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
        : HttpHandlerBase(config, component_context),
            pg_cluster_(
                component_context
                    .FindComponent<userver::components::Postgres>("postgres-db-1")
                    .GetCluster()) {}

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&
    ) const override {
        auto session = GetSessionInfo(pg_cluster_, request);
        if (!session) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
            return {};
        }
        
        std::optional<int> queue_id = helpers::GetIntValueByKey(request, "id");
        if (!queue_id.has_value()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        auto request_body = userver::formats::json::FromString(request.RequestBody());
        auto name = request_body["name"].As<std::optional<std::string>>();

        if (!name.has_value()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        if (!IsQueueExist(pg_cluster_, *queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        auto result = UpdateQueueById(pg_cluster_, *queue_id, *name);
        auto queue = result.AsSingleRow<TQueue>(userver::storages::postgres::kRowTag);
        return ToString(userver::formats::json::ValueBuilder{queue}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUpdateQueue(userver::components::ComponentList& component_list) {
  component_list.Append<UpdateQueue>();
}

}  // namespace tracker
