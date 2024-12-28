#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <userver/chaotic/io/userver/utils/datetime/time_point_tz.hpp>
#include <userver/chaotic/type_bundle_hpp.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include <userver/utils/trivial_map.hpp>


namespace tracker
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    struct TTicket {
        enum class Priority { kLow, kMedium, kHigh };
        static constexpr Priority kPriorityValues[] = { Priority::kLow, Priority::kMedium, Priority::kHigh };
        std::string id { };
		std::string title { };
        std::optional<std::string> description { };
        int author_id { };
        std::optional<int> performer_id { };
        tracker::TTicket::Priority priority { tracker::TTicket::Priority::kLow };
        int queue_id { };
        int status_id { };
        std::optional<TimePoint> start_date { };
        std::optional<TimePoint> finish_date { };
        std::optional<TimePoint> deadline_date { };
        std::optional<TimePoint> created_date { };
        std::optional<TimePoint> deleted_date { };
    };


    bool operator==(const tracker::TTicket & lhs, const tracker::TTicket & rhs);
    userver::logging::LogHelper& operator<<(userver::logging::LogHelper& lh, const tracker::TTicket::Priority& value);
    userver::logging::LogHelper& operator<<(userver::logging::LogHelper& lh, const tracker::TTicket& value);
    TTicket::Priority Parse(const userver::formats::json::Value &json, userver::formats::parse::To<tracker::TTicket::Priority>);
    TTicket Parse(const userver::formats::json::Value &json, userver::formats::parse::To<tracker::TTicket>);
    TTicket::Priority FromString(std::string_view value, userver::formats::parse::To<tracker::TTicket::Priority>);
    TTicket::Priority Parse(std::string_view value, userver::formats::parse::To<tracker::TTicket::Priority>);
    userver::formats::json::Value Serialize(const tracker::TTicket::Priority& value, userver::formats::serialize::To<userver::formats::json::Value>);
    userver::formats::json::Value Serialize(const tracker::TTicket& value, userver::formats::serialize::To<userver::formats::json::Value>);
    std::string ToString(tracker::TTicket::Priority value);

} // namespace tracker

	template <>
	struct userver::storages::postgres::io::CppToUserPg<tracker::TTicket::Priority> {
  	static constexpr userver::storages::postgres::DBTypeName postgres_name = "tracker.priority_type";
  	static constexpr userver::utils::TrivialBiMap enumerators =
      [](auto selector) {
        return selector()
            .Case("LOW", tracker::TTicket::Priority::kLow)
            .Case("MEDIUM", tracker::TTicket::Priority::kMedium)
            .Case("HIGH", tracker::TTicket::Priority::kHigh);
      };
	};