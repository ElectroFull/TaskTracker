#pragma once
#include "queue.hpp"
#include <userver/chaotic/exception.hpp>
#include <userver/chaotic/object.hpp>
#include <userver/chaotic/primitive.hpp>
#include <userver/chaotic/validators.hpp>
#include <userver/chaotic/with_type.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/utils/trivial_map.hpp>

namespace tracker
{
	
	static constexpr userver::utils::TrivialSet ktracker__TQueue_PropertiesNames = [](auto selector) {
        return selector().template Type<std::string_view>().Case("id").Case("name").Case("tickets_count").Case("author_id").Case("created_date").Case("deleted_date");
    };

	template<typename Value>
    tracker::TQueue Parse(Value value, userver::formats::parse::To<tracker::TQueue>) {
        value.CheckNotMissing();
        value.CheckObjectOrNull();
        tracker::TQueue res;
        res.id = value["id"].template As<userver::chaotic::Primitive<int>>();
        res.name = value["name"].template As<userver::chaotic::Primitive<std::string>>();
        res.author_id = value["author_id"].template As<userver::chaotic::Primitive<int>>();
        res.tickets_count = value["tickets_count"].template As<userver::chaotic::Primitive<int>>();
        res.created_date = value["created_date"].template As<userver::chaotic::WithType<userver::chaotic::Primitive<std::string>, userver::utils::datetime::TimePointTz>>();
        res.deleted_date = value["deleted_date"].template As<std::optional<userver::chaotic::WithType<userver::chaotic::Primitive<std::string>, userver::utils::datetime::TimePointTz>>>();
        userver::chaotic::ValidateNoAdditionalProperties(value, ktracker__TQueue_PropertiesNames);
        return res;
    }
}