#pragma once
#include "ticket.hpp"

namespace tracker {

    struct TTicketFromUser {
		std::string title;
        std::optional<std::string> description;
        std::optional<int> performer_id;
        TTicket::Priority priority { TTicket::Priority::kLow };
        int queue_id;
        int status_id;
        std::optional<TimePoint> deadline_date;
    };
    TTicketFromUser Parse(const userver::formats::json::Value &value, userver::formats::parse::To<TTicketFromUser>);

} // namespace tracker