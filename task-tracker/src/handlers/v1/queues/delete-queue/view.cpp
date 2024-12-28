#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../lib/auth.hpp"
#include "../../../../helpers/helpers.hpp"
#include "../../../../queries/queues_queries.hpp"

namespace tracker {

namespace {

class DeleteQueue final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-delete-queue";

    DeleteQueue(const userver::components::ComponentConfig& config,
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
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kUnauthorized);
        }

        std::optional<int> queue_id = helpers::GetIntValueByKey(request, "id");
        if (!queue_id.has_value()) {
            helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        if (!HasAccessToQueue(pg_cluster_, session.value().user_id, *queue_id)) {
            helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }
        auto result = DeleteQueueById(pg_cluster_, *queue_id);

        userver::formats::json::ValueBuilder response;
        response["id"] = *queue_id;
        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendDeleteQueue(userver::components::ComponentList& component_list) {
    component_list.Append<DeleteQueue>();
}

}  // namespace tracker
