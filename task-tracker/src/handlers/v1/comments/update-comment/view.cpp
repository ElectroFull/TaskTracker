#include "view.hpp"

#include <fmt/format.h>

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

class UpdateComment final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-update-comment";

    UpdateComment(const userver::components::ComponentConfig& config,
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
        std::optional<int> comment_id = helpers::GetIntValueByKey(request, "id");
        if (!comment_id.has_value()){
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }
        auto request_body = userver::formats::json::FromString(request.RequestBody());
        auto message = request_body["message"].As<std::optional<std::string>>();
        if (!message.has_value()){
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }
        auto update = UpdateCommentById(pg_cluster_, *comment_id, *message);
        if (update.IsEmpty()){
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }
        auto new_comment = update.AsSingleRow<TComment>(userver::storages::postgres::kRowTag);
        return ToString(userver::formats::json::ValueBuilder{new_comment}.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUpdateComment(userver::components::ComponentList& component_list) {
    component_list.Append<UpdateComment>();
}

}  // namespace tracker

