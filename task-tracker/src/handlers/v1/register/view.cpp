#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../helpers/helpers.hpp"
#include "../../../queries/user_queries.hpp"

namespace tracker {

namespace {

class RegisterUser final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-v1-register-user";

    RegisterUser(const userver::components::ComponentConfig& config,
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

        // if (!helpers::IsEmailValid(login)) {
        //     return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kBadRequest);
        // }

        auto userResult = tracker::GetAllQueries(pg_cluster_, login);

        if (!userResult.IsEmpty()) {
            return helpers::FillEmptyResponseWithStatus(request, userver::server::http::HttpStatus::kConflict);
        }

        auto result = tracker::RegisterUserQuery(pg_cluster_, login, password);

        userver::formats::json::ValueBuilder response;
        response["id"] = result.AsSingleRow<int>();

        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendRegisterUser(userver::components::ComponentList& component_list) {
    component_list.Append<RegisterUser>();
}

}  // namespace tracker
