#pragma once

#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include <string_view>

namespace tracker 
{
userver::storages::postgres::ResultSet GetAllPerformerLoginsFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int queue_id);

userver::storages::postgres::ResultSet GetAllAuthorLoginsFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int queue_id);

userver::storages::postgres::ResultSet GetUserLogin(userver::storages::postgres::ClusterPtr _pg_cluster, int id);

userver::storages::postgres::ResultSet GetAllQueries(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string_view& login);

userver::storages::postgres::ResultSet RegisterUserQuery(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string_view& login, const std::string_view& password);

userver::storages::postgres::ResultSet LoginUserQuery(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id);

userver::storages::postgres::ResultSet UnregisterUserQuery(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id);

userver::storages::postgres::ResultSet GetUsersByTicketId(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string& ticket_id);

userver::storages::postgres::ResultSet AddUserIdToTicket(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, const std::string& ticket_id);

userver::storages::postgres::ResultSet DeleteUserIdFromTicket(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, const std::string& ticket_id);

userver::storages::postgres::ResultSet GetAllUsers(userver::storages::postgres::ClusterPtr _pg_cluster);

userver::storages::postgres::ResultSet GetUserByUserLogin(userver::storages::postgres::ClusterPtr _pg_cluster, const std::string& login);

bool IsUserExist(userver::storages::postgres::ClusterPtr _pg_cluster, int id);

}