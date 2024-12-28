#pragma once
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include "../models/ticket.hpp"
#include "../models/ticket.ipp"

namespace tracker 
{

userver::storages::postgres::ResultSet AddUserToQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int queue_id, int author_id);
userver::storages::postgres::ResultSet DeleteUserFromQueue(userver::storages::postgres::ClusterPtr _pg_cluster, int user_id, int queue_id);
userver::storages::postgres::ResultSet DeleteTicket(userver::storages::postgres::ClusterPtr _pg_cluster, int id);
userver::storages::postgres::ResultSet GetTicket(userver::storages::postgres::ClusterPtr _pg_cluster, const TTicket &new_ticket);
userver::storages::postgres::ResultSet UpdateTicket(userver::storages::postgres::ClusterPtr _pg_cluster, const TTicket &ticket);
userver::storages::postgres::ResultSet AddComment(userver::storages::postgres::ClusterPtr _pg_cluster, int ticket_id, const std::string &message);
userver::storages::postgres::ResultSet GetAllComments(userver::storages::postgres::ClusterPtr _pg_cluster, int ticket_id);
userver::storages::postgres::ResultSet GetComment(userver::storages::postgres::ClusterPtr _pg_cluster, int id);
userver::storages::postgres::ResultSet UpdateComment(userver::storages::postgres::ClusterPtr _pg_cluster, int id, const std::string &message);
userver::storages::postgres::ResultSet DeleteComment(userver::storages::postgres::ClusterPtr _pg_cluster, int id);

} // namespace tracker