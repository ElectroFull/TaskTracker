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
#include "../../../../models/user_ticket.hpp"
#include "../../../../queries/user_queries.hpp"
#include "../../../../queries/queues_queries.hpp"

namespace tracker {

namespace {

class AddUserToTicket final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-add-watcher-to-ticket";

    AddUserToTicket(const userver::components::ComponentConfig& config,
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
        if (!request.HasPathArg("ticket_id") || !user_id.has_value()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }
        std::string ticket_id = request.GetPathArg("ticket_id");

        if (!IsUserExist(pg_cluster_, *user_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        if (!HasAccessToQueueByTicketId(pg_cluster_, session.value().user_id, ticket_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kForbidden);
        }
        auto result = AddUserIdToTicket(pg_cluster_, *user_id, ticket_id);
        auto userTicket = result.AsSingleRow<TUserTicket>(userver::storages::postgres::kRowTag);
        return ToString(userver::formats::json::ValueBuilder{userTicket}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendAddUserToTicket(userver::components::ComponentList& component_list) {
  component_list.Append<AddUserToTicket>();
}

}  // namespace tracker