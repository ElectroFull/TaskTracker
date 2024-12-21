#include "ticket_from_user.hpp"
#include <userver/chaotic/type_bundle_cpp.hpp>

namespace tracker
{

    TTicketFromUser Parse(const userver::formats::json::Value &value, userver::formats::parse::To<TTicketFromUser>) {
        TTicketFromUser res;
        res.title = value["title"].As<std::string>();
		if (value.HasMember("description")) {
        	res.description = value["description"].As<std::optional<std::string>>();
		}
		if (value.HasMember("performer_id")) {
        	res.performer_id = value["performer_id"].As<std::optional<int>>();
		}
		if (value.HasMember("priority")) {
        	res.priority = value["priority"].As<TTicket::Priority>();
		}
        res.queue_id = value["queue_id"].As<int>();
        res.status_id = value["status_id"].As<int>();
		if (value.HasMember("deadline_date")) {
        	res.deadline_date = value["deadline_date"].As<std::optional<TimePoint>>();
		}
        return res;
    }

} // namespace tracker