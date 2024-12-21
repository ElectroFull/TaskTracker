#include "comments_queries.hpp"

namespace tracker
{

userver::storages::postgres::ResultSet GetAllComments(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string &ticket_id){
	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT id, message, ticket_id, created_date, deleted_date FROM tracker.comments "
        "WHERE ticket_id = $1 AND deleted_date IS NULL;",
        ticket_id
	);
	return result;
}

userver::storages::postgres::ResultSet GetCommentById(userver::storages::postgres::ClusterPtr _pg_cluster, int id) {
	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT id, message, ticket_id, created_date, deleted_date FROM tracker.comments "
        "WHERE id = $1 AND deleted_date IS NULL;",
        id
	);
	return result;
}

userver::storages::postgres::ResultSet AddComment(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string &ticket_id, const std::string &message){

	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO tracker.comments(ticket_id, message, created_date) "
        "VALUES($1, $2, CURRENT_TIMESTAMP) "
		"ON CONFLICT DO NOTHING "
		"RETURNING *;",
        ticket_id, message
	);
	return result;
}


userver::storages::postgres::ResultSet UpdateCommentById(userver::storages::postgres::ClusterPtr _pg_cluster, int id, const std::string &message){
	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"WITH ins AS ( "
		"UPDATE tracker.comments "
		"SET message = $1 "
		"WHERE id = $2 AND deleted_date IS NULL "
		"RETURNING id, message, ticket_id, created_date, deleted_date "
        ") "
        "SELECT id, message, ticket_id, created_date, deleted_date FROM ins;",
        message, id
	);
	
	return result;
}

void DeleteCommentById(userver::storages::postgres::ClusterPtr _pg_cluster, int id){
	auto result = _pg_cluster->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		"UPDATE tracker.comments "
		"SET deleted_date = CURRENT_TIMESTAMP "
		"WHERE id = $1;",
		id
	);
}

}