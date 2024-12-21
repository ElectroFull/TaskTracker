#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../../models/ticket.hpp"
#include "../../../../models/ticket.ipp"
#include "../../../../models/ticket_from_user.hpp"
#include "../../../../queries/statuses.hpp"
#include "../../../../queries/ticket_queries.hpp"
#include "../../../../queries/user_queries.hpp"
#include "../../../../queries/queues_queries.hpp"
#include "../../../lib/auth.hpp"
#include "../../../../helpers/helpers.hpp"


namespace tracker {

namespace {

class GetTicket final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-ticket";

    GetTicket(const userver::components::ComponentConfig& config,
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
        
        std::string ticket_id = request.GetPathArg("id");
        if (!HasAccessToQueueByTicketId(pg_cluster_, session.value().user_id, ticket_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kForbidden);
        }
        auto result = GetTicketById(pg_cluster_, ticket_id);
        if (result.IsEmpty()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        
        auto ticket = result.AsSingleRow<TTicket>(userver::storages::postgres::kRowTag);
        auto response = userver::formats::json::ValueBuilder{ticket};
        response["author_login"] = GetUserLogin(pg_cluster_, ticket.author_id).AsSingleRow<std::string>(); 
        if (ticket.performer_id) {
            response["performer_login"] = GetUserLogin(pg_cluster_, *ticket.performer_id).AsSingleRow<std::string>();
        }
        return ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace


void AppendGetTicket(userver::components::ComponentList& component_list) {
    component_list.Append<GetTicket>();
}

}  // namespace tracker
