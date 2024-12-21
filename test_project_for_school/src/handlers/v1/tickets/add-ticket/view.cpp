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
#include "../../../../queries/queues_queries.hpp"
#include "../../../../queries/user_queries.hpp"
#include "../../../lib/auth.hpp"
#include "../../../../helpers/helpers.hpp"



namespace tracker {

namespace {

class CreateTicket final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-create-ticket";

    CreateTicket(const userver::components::ComponentConfig& config,
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

        auto json = userver::formats::json::FromString(request.RequestBody());
        TTicketFromUser ticket;
        try {
            ticket = json.As<TTicketFromUser>();
        } catch (...) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        if (!IsQueueExist(pg_cluster_, ticket.queue_id)){
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        if (!HasAccessToQueue(pg_cluster_, session.value().user_id, ticket.queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kForbidden);
        }
        if (ticket.performer_id.has_value() && !IsUserExist (pg_cluster_, *ticket.performer_id)){
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        if (ticket.performer_id.has_value() && !HasAccessToQueue(pg_cluster_ , *ticket.performer_id, ticket.queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kForbidden);
        }
        if (!IsValidStatus(pg_cluster_, ticket.status_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }
        auto result = AddTicket(pg_cluster_, ticket, session.value().user_id);
        if (result.IsEmpty()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kConflict);
        }
        userver::formats::json::ValueBuilder response;
        response["id"] = result.AsSingleRow<std::string>();
        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace


void AppendCreateTicket(userver::components::ComponentList& component_list) {
    component_list.Append<CreateTicket>();
}

}  // namespace tracker
