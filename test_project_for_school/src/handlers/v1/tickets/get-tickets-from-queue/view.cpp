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
#include "../../../../queries/ticket_queries.hpp"
#include "../../../../queries/user_queries.hpp"
#include "../../../../queries/queues_queries.hpp"
#include "../../../lib/auth.hpp"
#include "../../../../helpers/helpers.hpp"


namespace tracker {

namespace {

class GetTicketsFromQueue final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-tickets-from-queue";

    GetTicketsFromQueue(const userver::components::ComponentConfig& config,
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
        if (!queue_id) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }
        if (!HasAccessToQueue(pg_cluster_, session.value().user_id, *queue_id)) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kForbidden);
        }

        auto tickets = GetAllTicketsFromQueue(pg_cluster_, *queue_id);
        auto authors = GetAllAuthorLoginsFromQueue(pg_cluster_, *queue_id);
        auto performers = GetAllPerformerLoginsFromQueue(pg_cluster_, *queue_id);
        if (authors.Size() != tickets.Size() || performers.Size() != tickets.Size()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kInternalServerError);
        }
        auto response = FillResponse(ResultSetToVector(authors), ResultSetToVector(performers), tickets);
        return ToString(response.ExtractValue());
    }

private:
    std::vector<std::string> ResultSetToVector(const userver::storages::postgres::ResultSet &result) const {
        std::vector<std::string> logins;
        for (auto row : result.AsSetOf<std::string>()) {
            logins.push_back(row);
        }
        return logins;
    }

    userver::formats::json::ValueBuilder FillResponse(const std::vector<std::string> &all_authors,
        const std::vector<std::string> &all_performers,
        const userver::storages::postgres::ResultSet &result) const {
        userver::formats::json::ValueBuilder response;
        response["items"].Resize(0);
        int i = 0;
        for (auto row : result.AsSetOf<TTicket>(userver::storages::postgres::kRowTag)) {
            response["items"].PushBack(row);
            response["items"][i]["author_login"] = std::move(all_authors[i]);
            if (row.performer_id) {
                response["items"][i]["performer_login"] = std::move(all_performers[i]);
            }
            ++i;
        }
        return response;
    }
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace


void AppendGetTicketsFromQueue(userver::components::ComponentList& component_list) {
    component_list.Append<GetTicketsFromQueue>();
}

}  // namespace tracker
