#pragma once

#include <userver/formats/json/value_builder.hpp>

namespace tracker {

struct TUserQueue {
    int user_id;
    int queue_id;
};

userver::formats::json::Value Serialize(const tracker::TUserQueue& userQueue, userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace tracker