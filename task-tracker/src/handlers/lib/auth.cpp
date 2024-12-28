#include "auth.hpp"

namespace tracker {

std::optional<TSession> GetSessionInfo(
    userver::storages::postgres::ClusterPtr pg_cluster,
    const userver::server::http::HttpRequest& request
) {
    if (!request.HasHeader(USER_TICKET_HEADER_NAME)) {
        LOG_INFO() << "USER_TICKET_HEADER_NAME NOT FOUND";
        return std::nullopt;
    }
    auto id = request.GetHeader(USER_TICKET_HEADER_NAME);

    auto result = pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM tracker.auth_sessions "
        "WHERE id = $1;",
        id
    );
    if (result.IsEmpty()) {
        LOG_INFO() << "RESULT IS EMPTY";
        return std::nullopt;
    }

     return result.AsSingleRow<TSession>(userver::storages::postgres::kRowTag);
}

}  // namespace tracker