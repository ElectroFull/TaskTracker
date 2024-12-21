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
#include "../../../../models/user_queue.hpp"
#include "../../../../queries/queues_queries.hpp"
#include "../../../../queries/user_queries.hpp"

namespace tracker {

namespace {

class DeleteUserFromQueue final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-delete-user-from-queue";

    DeleteUserFromQueue(const userver::components::ComponentConfig& config,
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
        std::optional<int> user_id = helpers::GetIntValueByKey(request, "user_id");
        std::optional<int> queue_id = helpers::GetIntValueByKey(request, "queue_id");
        if (!user_id.has_value() || !queue_id.has_value()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }
        if (!IsQueueExist(pg_cluster_, *queue_id) || !IsUserExist(pg_cluster_, *user_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        if (!HasAccessToQueue(pg_cluster_, session.value().user_id, *queue_id) ||
            !IsUserInQueue(pg_cluster_, *user_id, *queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        auto result = DeleteUserIdFromQueue(pg_cluster_, *user_id, *queue_id);
        auto userQueue = result.AsSingleRow<TUserQueue>(userver::storages::postgres::kRowTag);
        return ToString(userver::formats::json::ValueBuilder{userQueue}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendDeleteUserFromQueue(userver::components::ComponentList& component_list) {
    component_list.Append<DeleteUserFromQueue>();
}

}  // namespace tracker
