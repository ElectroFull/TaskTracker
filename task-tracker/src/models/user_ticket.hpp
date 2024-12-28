#pragma once

#include <userver/formats/json/value_builder.hpp>

#include <string>

namespace tracker {

struct TUserTicket {
    int user_id;
    std::string ticket_id;
};

userver::formats::json::Value Serialize(const tracker::TUserTicket& userTicket, userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace tracker