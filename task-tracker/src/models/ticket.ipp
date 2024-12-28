#pragma once
#include "ticket.hpp"
#include <userver/chaotic/exception.hpp>
#include <userver/chaotic/object.hpp>
#include <userver/chaotic/primitive.hpp>
#include <userver/chaotic/validators.hpp>
#include <userver/chaotic/with_type.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/utils/trivial_map.hpp>
#include <userver/utils/datetime/date.hpp>
#include <userver/chaotic/convert.hpp>
namespace tracker
{
    static constexpr userver::utils::TrivialBiMap ktracker__TTicket__Priority_Mapping = [](auto selector) {
        return selector().template Type<tracker::TTicket::Priority, std::string_view>()
            .Case(tracker::TTicket::Priority::kLow, "LOW")
            .Case(tracker::TTicket::Priority::kMedium, "MEDIUM")
            .Case(tracker::TTicket::Priority::kHigh, "HIGH");
    };

	static constexpr userver::utils::TrivialSet ktracker__TTicket_PropertiesNames = [](auto selector) {
    return selector().template Type<std::string_view>().Case("id").Case("title").Case("description").Case("author_id").Case("performer_id").Case("priority").Case("queue_id").Case("status_id").Case("start_date").Case("finish_date").Case("deadline_date").Case("created_date").Case("deleted_date");
    };
} // namespace tracker