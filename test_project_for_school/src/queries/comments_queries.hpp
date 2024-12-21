#pragma once

#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace tracker 
{

userver::storages::postgres::ResultSet GetAllComments(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string &ticket_id);
userver::storages::postgres::ResultSet GetCommentById(userver::storages::postgres::ClusterPtr _pg_cluster, int id);
userver::storages::postgres::ResultSet AddComment(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string &ticket_id, const std::string &message);
userver::storages::postgres::ResultSet UpdateCommentById(userver::storages::postgres::ClusterPtr _pg_cluster, int id, const std::string &message);
void DeleteCommentById(userver::storages::postgres::ClusterPtr _pg_cluster, int id);

}