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
void GetUpdatedTicket(userver::formats::json::Value &value, TTicket &ticket, userver::storages::postgres::ClusterPtr pg_cluster) {
		if (value.HasMember("title")) {
            ticket.title = value["title"].As<std::string>();
        }
		if (value.HasMember("description")) {
        	ticket.description = value["description"].As<std::optional<std::string>>();
		}
		if (value.HasMember("priority")) {
        	ticket.priority = value["priority"].As<TTicket::Priority>();
		}
		if (value.HasMember("status_id")) {
            ticket.status_id = value["status_id"].As<int>();
            if (!IsValidStatus(pg_cluster, ticket.status_id)) {
                throw userver::server::http::HttpStatus::kForbidden;
            }
        }   
        if (value.HasMember("performer_id")) {
            ticket.performer_id = value["performer_id"].As<int>();
        }
		if (value.HasMember("deadline_date")) {
        	ticket.deadline_date = value["deadline_date"].As<std::optional<TimePoint>>();
		}
        if (value.HasMember("finish_date")) {
        	ticket.finish_date = value["finish_date"].As<std::optional<TimePoint>>();
		}
    }
namespace {

class UpdateTicket final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-update-ticket";

    UpdateTicket(const userver::components::ComponentConfig& config,
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

        auto json = userver::formats::json::FromString(request.RequestBody());
        try {
            GetUpdatedTicket(json, ticket, pg_cluster_);
        } catch (...) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        if (ticket.performer_id && !HasAccessToQueue(pg_cluster_, *ticket.performer_id, ticket.queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kForbidden);
        }

        result = UpdateTicketsFields(pg_cluster_, ticket);
        if (result.IsEmpty()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kInternalServerError);
        }

        userver::formats::json::ValueBuilder response;
        response["id"] = result.AsSingleRow<std::string>();
        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace


void AppendUpdateTicket(userver::components::ComponentList& component_list) {
    component_list.Append<UpdateTicket>();
}

}  // namespace tracker
