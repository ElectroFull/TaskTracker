#include "user_queries.hpp"

namespace tracker
{

userver::storages::postgres::ResultSet GetAllQueries(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string_view& login) {
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT id, login, password, created_date, deleted_date FROM tracker.users "
        "WHERE login = $1 AND deleted_date IS NULL",
        login
    );
}

userver::storages::postgres::ResultSet RegisterUserQuery(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string_view& login, const std::string_view& password) {
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO tracker.users(login, password, created_date) VALUES($1, $2, CURRENT_TIMESTAMP) "
        "ON CONFLICT DO NOTHING "
        "RETURNING users.id",
        login, password
    );
}

userver::storages::postgres::ResultSet LoginUserQuery(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id) {
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO tracker.auth_sessions(user_id) VALUES($1) "
        "ON CONFLICT DO NOTHING "
        "RETURNING auth_sessions.id",
        user_id
    );
}

userver::storages::postgres::ResultSet UnregisterUserQuery(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id) {
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE tracker.users "
        "SET deleted_date = CURRENT_TIMESTAMP "
        "WHERE id = $1 AND deleted_date IS NULL "
        "RETURNING users.id",
        user_id
    );
}

userver::storages::postgres::ResultSet GetUsersByTicketId(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string& ticket_id) {
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT tracker.users.* "
        "FROM tracker.users "
        "INNER JOIN tracker.watchers ON tracker.users.id = tracker.watchers.user_id "
        "WHERE tracker.watchers.ticket_id = $1;",
        ticket_id
    );
}

userver::storages::postgres::ResultSet AddUserIdToTicket(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, const std::string& ticket_id) {
	return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"WITH ins AS ( "
		"INSERT INTO tracker.watchers(user_id, ticket_id) "
		"VALUES($1, $2) "
		"ON CONFLICT DO NOTHING "
		"RETURNING * "
		") "
		"SELECT * FROM tracker.watchers WHERE user_id = $1 AND ticket_id = $2 "
        "UNION ALL "
        "SELECT * FROM ins",
		user_id, ticket_id
	);
}

userver::storages::postgres::ResultSet DeleteUserIdFromTicket(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, const std::string& ticket_id) {
    return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"DELETE FROM tracker.watchers "
		"WHERE user_id = $1 AND ticket_id = $2 "
		"RETURNING * ;",
		user_id, ticket_id
	);
}


bool IsUserExist(userver::storages::postgres::ClusterPtr _pg_cluster, int id) {
    auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT id FROM tracker.users "
        "WHERE tracker.users.id = $1 AND deleted_date IS NULL;",
        id
	);
    return !result.IsEmpty();
}


userver::storages::postgres::ResultSet GetUserLogin(userver::storages::postgres::ClusterPtr _pg_cluster, int id) {
    auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT login FROM tracker.users "
        "WHERE tracker.users.id = $1 AND deleted_date IS NULL;",
        id
	);
    return result;
}

userver::storages::postgres::ResultSet GetAllPerformerLoginsFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int queue_id) {
    auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT COALESCE(u.login, '') AS login "
        "FROM tracker.tickets t "
        "LEFT JOIN tracker.users u ON t.performer_id = u.id "
        "WHERE t.queue_id = $1 AND t.deleted_date IS NULL "
        "ORDER BY t.id;",
        queue_id
	);
    return result;
}

userver::storages::postgres::ResultSet GetAllAuthorLoginsFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int queue_id) {
    auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT u.login "
        "FROM tracker.tickets t "
        "JOIN tracker.users u ON t.author_id = u.id "
        "WHERE t.queue_id = $1 AND t.deleted_date IS NULL "
        "ORDER BY t.id;",
        queue_id
	);
    return result;
}

userver::storages::postgres::ResultSet GetAllUsers(userver::storages::postgres::ClusterPtr _pg_cluster) {
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * "
        "FROM tracker.users;"
    );
}

userver::storages::postgres::ResultSet GetUserByUserLogin(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string& login) {
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * "
        "FROM tracker.users "
        "WHERE login = $1;",
        login
    );
}

}