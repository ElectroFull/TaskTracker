#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../lib/auth.hpp"
#include "../../../../models/queue.hpp"
#include "../../../../helpers/helpers.hpp"
#include "../../../../queries/queues_queries.hpp"

namespace tracker {

namespace {

class AddQueue final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-add-queue";

    AddQueue(const userver::components::ComponentConfig& config,
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

        auto request_body = userver::formats::json::FromString(request.RequestBody());
        auto name = request_body["name"].As<std::optional<std::string>>();

        if (!name.has_value()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        auto result = CreateQueue(pg_cluster_, name.value(), session->user_id);
        if (result.IsEmpty()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kConflict);
        }
        auto queue = result.AsSingleRow<TQueue>(userver::storages::postgres::kRowTag);
        AddUserIdToQueue(pg_cluster_, session->user_id, queue.id);
        return ToString(userver::formats::json::ValueBuilder{queue}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendAddQueue(userver::components::ComponentList& component_list) {
    component_list.Append<AddQueue>();
}

}  // namespace tracker
