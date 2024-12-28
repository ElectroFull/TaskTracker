#pragma once
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace tracker 
{

userver::storages::postgres::ResultSet GetAllQueues(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id);
userver::storages::postgres::ResultSet GetQueueById(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id);
userver::storages::postgres::ResultSet CreateQueue(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string &name, int author_id);
userver::storages::postgres::ResultSet UpdateQueueById(userver::storages::postgres::ClusterPtr _pg_cluster, int id, const std::string& name);
userver::storages::postgres::ResultSet DeleteQueueById(userver::storages::postgres::ClusterPtr _pg_cluster, int id);
userver::storages::postgres::ResultSet AddUserIdToQueue(userver::storages::postgres::ClusterPtr _pg_clustre, int user_id, int queue_id);
userver::storages::postgres::ResultSet DeleteUserIdFromQueue(userver::storages::postgres::ClusterPtr _pg_clustre, int user_id, int queue_id);
userver::storages::postgres::ResultSet GetAllUsersFromQueue(userver::storages::postgres::ClusterPtr _pg_clustre, int queue_id);
bool HasAccessToQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id);
bool HasAccessToQueueByTicketId(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, const std::string& ticket_id);
bool IsQueueExist(userver::storages::postgres::ClusterPtr _pg_cluster, int id);
bool IsUserInQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id);

}