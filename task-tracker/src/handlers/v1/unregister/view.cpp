#include "view.hpp"

#include <string_view>
#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../models/user.hpp"
#include "../../../helpers/helpers.hpp"
#include "../../../queries/user_queries.hpp"

namespace tracker {

namespace {

class UnregisterUser final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-unregister-user";

    UnregisterUser(const userver::components::ComponentConfig& config,
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

        if (!request.HasFormDataArg("login") || !request.HasFormDataArg("password")) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        }

        auto login = request.GetFormDataArg("login").value;
        auto password = userver::crypto::hash::Sha256(request.GetFormDataArg("password").value);

        auto userResult = tracker::GetAllQueries(pg_cluster_, login);

        if (userResult.IsEmpty()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }

        auto user = userResult.AsSingleRow<TUser>(userver::storages::postgres::kRowTag);
        if (password != user.password) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kNotFound);
        }

        auto result = tracker::UnregisterUserQuery(pg_cluster_, user.id);
        
        userver::formats::json::ValueBuilder response;
        response["id"] = result.AsSingleRow<int>();

        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUnregisterUser(userver::components::ComponentList& component_list) {
    component_list.Append<UnregisterUser>();
}

}  // namespace tracker
