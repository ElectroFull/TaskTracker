#include "view.hpp"

#include <fmt/format.h>
#include <sstream>
#include <unordered_map>

#include <userver/components/component_context.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../../models/queue.hpp"
#include "../../../lib/auth.hpp"
#include "../../../../helpers/helpers.hpp"
#include "../../../../queries/queues_queries.hpp"

namespace tracker {

namespace {

class GetQueue final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-get-queue";

    GetQueue(const userver::components::ComponentConfig& config,
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
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        auto result = GetQueueById(pg_cluster_, session.value().user_id, *queue_id);
        if (result.IsEmpty()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }

        auto queue = result.AsSingleRow<TQueue>(userver::storages::postgres::kRowTag);
        return ToString(userver::formats::json::ValueBuilder{queue}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetQueue(userver::components::ComponentList& component_list) {
    component_list.Append<GetQueue>();
}

} // namespace tracker