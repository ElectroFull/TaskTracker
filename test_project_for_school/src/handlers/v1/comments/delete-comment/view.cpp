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

class DeleteComment final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-delete-comment";

    DeleteComment(const userver::components::ComponentConfig& config,
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
        if (!comment_id.has_value()) {
           return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        DeleteCommentById(pg_cluster_, *comment_id);
        userver::formats::json::ValueBuilder response;
        response["id"] = *comment_id;
        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendDeleteComment(userver::components::ComponentList& component_list) {
    component_list.Append<DeleteComment>();
}

}  // namespace tracker

