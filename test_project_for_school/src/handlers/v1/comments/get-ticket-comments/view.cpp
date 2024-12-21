#include "view.hpp"

#include <fmt/format.h>
#include <iostream>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../../models/comment.hpp"
#include "../../../lib/auth.hpp"
#include "../../../../queries/comments_queries.hpp"
#include "../../../../helpers/helpers.hpp"

namespace tracker {

    namespace {

        class GetTicketComments final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view kName = "handler-v1-get-ticket-comments";

            GetTicketComments(const userver::components::ComponentConfig& config,
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

                std::optional<std::string> ticket_id = request.GetPathArg("id");
                if (!ticket_id.has_value()) {
                    return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
                }

                auto result = GetAllComments(pg_cluster_, *ticket_id);
                userver::formats::json::ValueBuilder response;
                response["items"].Resize(0);
                for (auto row : result.AsSetOf<TComment>(userver::storages::postgres::kRowTag)) {
                    response["items"].PushBack(row);
                }

                return userver::formats::json::ToString(response.ExtractValue());
            }

        private:
            userver::storages::postgres::ClusterPtr pg_cluster_;
        };

    }  // namespace

    void AppendGetTicketComments(userver::components::ComponentList& component_list) {
        component_list.Append<GetTicketComments>();
    }

}  // namespace tracker
