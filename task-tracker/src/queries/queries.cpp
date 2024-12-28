#include "queries.hpp"

namespace tracker
{

userver::storages::postgres::ResultSet AddUserToQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int queue_id, int author_id){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO tracker.users_queues(user_id, queue_id) "
        "VALUES($1, $2) "
		"ON CONFLICT DO NOTHING;", //TODO need check if queue deleted
        author_id, queue_id
	);
	return result;
}

userver::storages::postgres::ResultSet DeleteUserFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM tracker.users_queues "
		"WHERE user_id = $1 AND queue_id = $2;",
        user_id, queue_id
	);
	return result;
}

userver::storages::postgres::ResultSet DeleteTicket(userver::storages::postgres::ClusterPtr _pg_cluster, int id){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"UPDATE tracker.tickets "
		"SET deleted_date = CURRENT_TIMESTAMP "
		"WHERE id = $1;",
		id
	);
	return result;

}

userver::storages::postgres::ResultSet GetTicket(userver::storages::postgres::ClusterPtr _pg_cluster, const TTicket &new_ticket){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT * FROM tracker.tickets "
		"WHERE id = $1;", 
		new_ticket.id
	);
	return result;
}

} // namespace tracker