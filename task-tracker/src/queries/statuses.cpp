#include "statuses.hpp"

namespace tracker
{

userver::storages::postgres::ResultSet CreateStatus(userver::storages::postgres::ClusterPtr _pg_cluster, int name){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO tracker.ticket_statuses(name) "
        "VALUES($1) "
		"ON CONFLICT DO NOTHING;", //TODO need check if queue deleted
        name
	);
	return result;
}

userver::storages::postgres::ResultSet GetDefaultStatus(userver::storages::postgres::ClusterPtr _pg_cluster){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * "
		"FROM tracker.ticket_statuses "
		"LIMIT 1;"
	);
	return result;
}

userver::storages::postgres::ResultSet GetStatus(userver::storages::postgres::ClusterPtr _pg_cluster, int id){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"SELECT * "
		"FROM tracker.ticket_statuses "
		"WHERE id = $1;",
		id
	);
	return result;
}

userver::storages::postgres::ResultSet DeleteStatus(userver::storages::postgres::ClusterPtr _pg_cluster, int id){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"UPDATE tracker.ticket_statuses "
        "SET deleted_date = CURRENT_TIMESTAMP "
        "WHERE id = $1 AND deleted_date IS NULL "
        "RETURNING ticket_statuses.id",
		id
	);
	return result;
}

} // namespace tracker