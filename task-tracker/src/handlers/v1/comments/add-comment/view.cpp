#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../lib/auth.hpp"
#include "../../../../models/comment.hpp"
#include "../../../../helpers/helpers.hpp"
#include "../../../../queries/comments_queries.hpp"

namespace tracker {

    namespace {

        class CreateComment final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view kName = "handler-v1-add-comment";

            CreateComment(const userver::components::ComponentConfig& config,
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

                if (!request.HasPathArg("id")) {
                    return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
                }
                std::string ticket_id = request.GetPathArg("id");

                auto message = request_body["message"].As<std::optional<std::string>>();
                if (!message.has_value()) {
                    return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
                }

                auto result = AddComment(pg_cluster_, ticket_id, message.value());

                auto comment = result.AsSingleRow<TComment>(userver::storages::postgres::kRowTag);
                return ToString(userver::formats::json::ValueBuilder{comment}.ExtractValue());
            }

        private:
            userver::storages::postgres::ClusterPtr pg_cluster_;
        };

    }  // namespace

    void AppendCreateComment(userver::components::ComponentList& component_list) {
        component_list.Append<CreateComment>();
    }

}  // namespace tracker
