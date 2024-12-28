#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <userver/chaotic/io/userver/utils/datetime/time_point_tz.hpp>
#include <userver/chaotic/type_bundle_hpp.hpp>

namespace tracker
{
    struct TComment {
        int id { };
		std::string message { };
		std::string ticket_id { }; 
        std::chrono::system_clock::time_point created_date{};
        std::optional<std::chrono::system_clock::time_point> deleted_date{};
    };
    bool operator==(const tracker::TComment & lhs, const tracker::TComment & rhs);
    userver::logging::LogHelper& operator<<(userver::logging::LogHelper& lh, const tracker::TComment& value);
    TComment Parse(userver::formats::json::Value json, userver::formats::parse::To<tracker::TComment>);
    userver::formats::json::Value Serialize(const tracker::TComment& value, userver::formats::serialize::To<userver::formats::json::Value>);
    
} // namespace tracker