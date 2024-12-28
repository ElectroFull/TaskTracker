#pragma once
#include <userver/chaotic/io/userver/utils/datetime/time_point_tz.hpp>
#include <userver/chaotic/type_bundle_hpp.hpp>

#include <chrono>
#include <string>

#include <userver/formats/json/value_builder.hpp>

namespace tracker {

struct TUser {
    int id;
    std::string login;
    std::string password;
    std::chrono::system_clock::time_point created_date;
    std::optional<std::chrono::system_clock::time_point> deleted_date;
};

TUser Parse(userver::formats::json::Value json, userver::formats::parse::To<tracker::TUser>);
userver::formats::json::Value Serialize(const tracker::TUser& user, userver::formats::serialize::To<userver::formats::json::Value>);

}