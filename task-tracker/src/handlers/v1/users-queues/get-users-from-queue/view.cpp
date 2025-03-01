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
#include "../../../../models/user.hpp"
#include "../../../../queries/queues_queries.hpp"
#include "../../../../queries/user_queries.hpp"
#include "../../../../queries/queues_queries.hpp"

namespace tracker {

namespace {

class GetUsersFromQueue final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-get-users-from-queue";

    GetUsersFromQueue(const userver::components::ComponentConfig& config,
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

        std::optional<int> queue_id = helpers::GetIntValueByKey(request, "queue_id");
        if (!queue_id.has_value()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        if (!IsQueueExist(pg_cluster_, *queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        if (!HasAccessToQueue(pg_cluster_, session.value().user_id, *queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }

        auto result = GetAllUsersFromQueue(pg_cluster_, *queue_id);
        userver::formats::json::ValueBuilder response;
        response["items"].Resize(0);
        for (auto row : result.AsSetOf<TUser>(userver::storages::postgres::kRowTag)) {
            response["items"].PushBack(row);
        }
        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetUsersFromQueue(userver::components::ComponentList& component_list) {
  component_list.Append<GetUsersFromQueue>();
}

}  // namespace tracker
