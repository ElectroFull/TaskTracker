#include "user_ticket.hpp"

namespace tracker {

userver::formats::json::Value Serialize(const tracker::TUserTicket& userTicket, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder vb = userver::formats::common::Type::kObject;

    vb["user_id"] = userTicket.user_id;
    vb["ticket_id"] = userTicket.ticket_id;

    return vb.ExtractValue();
}

}