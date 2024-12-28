#include "ticket_queries.hpp"
#include "queues_queries.hpp"
#include <userver/storages/postgres/parameter_store.hpp>
#include "../models/ticket.hpp"
#include "../models/ticket_from_user.hpp"

namespace tracker
{
	userver::storages::postgres::ResultSet GetAllTicketsFromQueue(userver::storages::postgres::ClusterPtr pg_cluster, int queue_id){
        auto result = pg_cluster->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "SELECT id, title, description, author_id, performer_id, "
            "priority, queue_id, status_id, start_date, finish_date, "
            "deadline_date, created_date, deleted_date "
            "FROM tracker.tickets "
            "WHERE queue_id = $1 AND deleted_date IS NULL "
			"ORDER BY id;",
            queue_id
        );
        return result;
	}
    userver::storages::postgres::ResultSet GetTicketById(userver::storages::postgres::ClusterPtr pg_cluster, const std::string &ticket_id){
        auto result = pg_cluster->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "SELECT id, title, description, author_id, performer_id, "
            "priority, queue_id, status_id, start_date, finish_date, "
            "deadline_date, created_date, deleted_date "
            "FROM tracker.tickets "
            "WHERE id = $1 AND deleted_date IS NULL;",
            ticket_id
        );
        return result;
	}

    bool IsValidStatus(userver::storages::postgres::ClusterPtr pg_cluster, int status_id) {
        auto result = pg_cluster->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			"SELECT 1 FROM tracker.ticket_statuses "
			"WHERE id = $1;",
			status_id
		);
        return !result.IsEmpty();
    }

    std::string GetTicketId(int queue_id, userver::storages::postgres::ClusterPtr pg_cluster) {
		using TupleType = std::tuple<std::string, int>;
		auto result = pg_cluster->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			"SELECT name, tickets_count FROM tracker.queues "
			"WHERE id = $1;",
			queue_id
		);
		auto tuple = result.AsSingleRow<TupleType>(userver::storages::postgres::kRowTag);
        std::string queue_name = std::get<0>(tuple);
        int ticket_num = std::get<1>(tuple) + 1;
		pg_cluster->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			"UPDATE tracker.queues "
			"SET tickets_count = $1 "
			"WHERE id = $2;",
			ticket_num, queue_id
		);
        return queue_name + "_" + std::to_string(ticket_num);
    }

	userver::storages::postgres::ResultSet AddTicket(userver::storages::postgres::ClusterPtr _pg_cluster, const TTicketFromUser &ticket, int author_id){
		std::string ticket_id = GetTicketId(ticket.queue_id, _pg_cluster);
		auto result = _pg_cluster->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			"INSERT INTO tracker.tickets "
			"( "
			"id, title, description, author_id, queue_id, priority, performer_id, "
			"status_id, created_date "
			") "
			"VALUES($1, $2, $3, $4, $5, $6, $7, $8, CURRENT_TIMESTAMP) "
			"ON CONFLICT DO NOTHING "
			"RETURNING id;",
			ticket_id, ticket.title, ticket.description, author_id, ticket.queue_id,
			ticket.priority, ticket.performer_id, ticket.status_id
		);
		return result;
	}

	userver::storages::postgres::ResultSet UpdateTicketsFields(userver::storages::postgres::ClusterPtr _pg_cluster, const TTicket &ticket){
		std::optional<userver::storages::postgres::TimePointWithoutTz> deadline;
		std::optional<userver::storages::postgres::TimePointWithoutTz> finish_date;
		if (ticket.deadline_date) {
			deadline = userver::storages::postgres::TimePointWithoutTz{*ticket.deadline_date};
		}
		if (ticket.finish_date) {
			finish_date = userver::storages::postgres::TimePointWithoutTz{*ticket.finish_date};
		}
		auto result = _pg_cluster->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			"UPDATE tracker.tickets "
			"SET title = $1, "
			"description = $2, "
			"performer_id = $3, "
			"priority = $4, "
			"status_id = $5, "
			"deadline_date = $6, "
			"finish_date = $7 "
			"WHERE id = $8 "
			"RETURNING id;",
			ticket.title, ticket.description, ticket.performer_id, ticket.priority,
			ticket.status_id, deadline, finish_date, ticket.id
		);
		return result;
}

} // namespace tracker