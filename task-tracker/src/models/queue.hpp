#pragma once
#include <userver/chaotic/io/userver/utils/datetime/time_point_tz.hpp>
#include <userver/chaotic/type_bundle_hpp.hpp>

#include <chrono>
#include <string>

#include <userver/formats/json/value_builder.hpp>

namespace tracker
{
    struct TQueue {
        int id { };
		std::string name { };
        int author_id{ };
		int tickets_count { };
        std::chrono::system_clock::time_point created_date;
        std::optional<std::chrono::system_clock::time_point> deleted_date;
    };

	bool operator==(const tracker::TQueue & lhs, const tracker::TQueue & rhs);
    userver::logging::LogHelper& operator<<(userver::logging::LogHelper& lh, const tracker::TQueue& value);
    TQueue Parse(userver::formats::json::Value json, userver::formats::parse::To<tracker::TQueue>);
    userver::formats::json::Value Serialize(const tracker::TQueue& value, userver::formats::serialize::To<userver::formats::json::Value>);
}