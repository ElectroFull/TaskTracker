#pragma once
#include "../models/ticket.hpp"
#include "../models/ticket.ipp"
#include "../models/ticket_from_user.hpp"
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace tracker
{
    userver::storages::postgres::ResultSet GetTicketById(userver::storages::postgres::ClusterPtr pg_cluster, const std::string &ticket_id);
	bool IsUserInQueue(userver::storages::postgres::ClusterPtr pg_cluster, int user_id, int queue_id);
    bool IsValidStatus(userver::storages::postgres::ClusterPtr pg_cluster, int status_id);
    std::string GetTicketId(int queue_id, userver::storages::postgres::ClusterPtr pg_cluster);
	userver::storages::postgres::ResultSet AddTicket(userver::storages::postgres::ClusterPtr _pg_cluster, const TTicketFromUser &ticket,  int author_id);
	userver::storages::postgres::ResultSet UpdateTicketsFields(userver::storages::postgres::ClusterPtr _pg_cluster, const TTicket &ticket);
	userver::storages::postgres::ResultSet GetAllTicketsFromQueue(userver::storages::postgres::ClusterPtr pg_cluster, int queue_id);
} // namespace tracker