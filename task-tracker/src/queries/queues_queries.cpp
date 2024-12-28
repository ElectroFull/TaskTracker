#include "queues_queries.hpp"

namespace tracker
{

userver::storages::postgres::ResultSet GetAllQueues(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id){
	return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT tracker.queues.* FROM tracker.queues "
        "LEFT JOIN tracker.users_queues ON tracker.queues.id = tracker.users_queues.queue_id "
        "WHERE (tracker.users_queues.user_id = $1 OR tracker.queues.author_id = $1) AND deleted_date IS NULL;",
        user_id
	);
}

userver::storages::postgres::ResultSet GetQueueById(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id) {
    return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT tracker.queues.* FROM tracker.queues "
        "LEFT JOIN tracker.users_queues ON tracker.queues.id = tracker.users_queues.queue_id "
        "WHERE (tracker.users_queues.user_id = $1 OR tracker.queues.author_id = $1) AND tracker.queues.id = $2 AND deleted_date IS NULL;",
        user_id, queue_id
	);
}

userver::storages::postgres::ResultSet CreateQueue(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string &name, int author_id){
	return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "WITH ins AS ( "
        "INSERT INTO tracker.queues(name, author_id, tickets_count, created_date) "
        "VALUES($1, $2, 0, CURRENT_TIMESTAMP) "
        "ON CONFLICT DO NOTHING "
        "RETURNING * "
        ") "
        "SELECT * FROM ins;",
        name, author_id
	);
}

userver::storages::postgres::ResultSet UpdateQueueById(userver::storages::postgres::ClusterPtr _pg_cluster, int id, const std::string& name) {
	return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"WITH ins AS ( "
        "UPDATE tracker.queues "
        "SET name = $2 "
        "WHERE id = $1 AND deleted_date IS NULL "
        "RETURNING * "
        ") "
        "SELECT * FROM ins;",
		id, name
	);
}

userver::storages::postgres::ResultSet DeleteQueueById(userver::storages::postgres::ClusterPtr _pg_cluster, int id){
	return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"UPDATE tracker.queues "
		"SET deleted_date = CURRENT_TIMESTAMP "
		"WHERE id = $1 AND deleted_date IS NULL;",
		id
	);
}

userver::storages::postgres::ResultSet AddUserIdToQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id) {
	return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"WITH ins AS ( "
		"INSERT INTO tracker.users_queues(user_id, queue_id) "
		"VALUES($1, $2) "
		"ON CONFLICT DO NOTHING "
		"RETURNING * "
		") "
		"SELECT * FROM tracker.users_queues WHERE user_id = $1 AND queue_id = $2 "
        "UNION ALL "
        "SELECT * FROM ins",
		user_id, queue_id
	);
}

userver::storages::postgres::ResultSet DeleteUserIdFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id) {
	return _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"DELETE FROM tracker.users_queues "
		"WHERE user_id = $1 AND queue_id = $2 "
		"RETURNING * ;",
		user_id, queue_id
	);
}

userver::storages::postgres::ResultSet GetAllUsersFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int queue_id)
{
    return _pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT tracker.users.* "
		"FROM tracker.users_queues "
		"INNER JOIN tracker.users ON tracker.users.id = tracker.users_queues.user_id "
        "WHERE tracker.users_queues.queue_id = $1 "
		"UNION "
		"SELECT tracker.users.* "
		"FROM tracker.users "
		"INNER JOIN tracker.queues ON tracker.queues.author_id = tracker.users.id "
		"WHERE tracker.queues.id = $1;",
        queue_id
    );
}

bool HasAccessToQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id) {
    auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT tracker.queues.* FROM tracker.queues "
        "LEFT JOIN tracker.users_queues ON tracker.queues.id = tracker.users_queues.queue_id "
        "WHERE (tracker.users_queues.user_id = $1 OR tracker.queues.author_id = $1) AND tracker.queues.id = $2 AND deleted_date IS NULL;",
        user_id, queue_id
	);

    return !result.IsEmpty();
}

bool HasAccessToQueueByTicketId(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, const std::string& ticket_id) {
    auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT tracker.tickets.id FROM tracker.tickets "
		"INNER JOIN tracker.queues ON tracker.queues.id = tracker.tickets.queue_id "
        "LEFT JOIN tracker.users_queues ON tracker.users_queues.queue_id = tracker.tickets.queue_id "
        "WHERE tracker.tickets.id = $2 AND "
		"(tracker.users_queues.user_id = $1 OR tracker.queues.author_id = $1) "
		"AND tracker.queues.deleted_date IS NULL;",
        user_id, ticket_id
	);

    return !result.IsEmpty();
}

bool IsQueueExist(userver::storages::postgres::ClusterPtr _pg_cluster, int id) {
    auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT id FROM tracker.queues "
        "WHERE tracker.queues.id = $1 AND deleted_date IS NULL;",
        id
	);
    return !result.IsEmpty();
}

bool IsUserInQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id) {
	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT user_id FROM tracker.users_queues "
		"WHERE user_id = $1 AND queue_id = $2;",
		user_id, queue_id
	);
	return !result.IsEmpty();
}

}