#pragma once
#include "comment.hpp"
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
    static constexpr userver::utils::TrivialSet ktracker__TComment_PropertiesNames = [](auto selector) {
        return selector().template Type<std::string_view>().Case("id").Case("message").Case("ticket_id").Case("created_date").Case("deleted_date");
    };
	
	template<typename Value>
    tracker::TComment Parse(Value value, userver::formats::parse::To<tracker::TComment>) {
        value.CheckNotMissing();
        value.CheckObjectOrNull();
        tracker::TComment res;
        res.id = value["id"].template As<userver::chaotic::Primitive<int>>(); 
        res.message = value["message"].template As<userver::chaotic::Primitive<std::string>>(); 
        res.ticket_id = value["ticket_id"].template As<userver::chaotic::Primitive<std::string>>(); 
        res.created_date = value["created_date"].template As<userver::chaotic::WithType<userver::chaotic::Primitive<std::string>, userver::utils::datetime::TimePointTz>>();
        res.deleted_date = value["deleted_date"].template As<std::optional<userver::chaotic::WithType<userver::chaotic::Primitive<std::string>, userver::utils::datetime::TimePointTz>>>();
        userver::chaotic::ValidateNoAdditionalProperties(value, ktracker__TComment_PropertiesNames);
        return res;
    }

} // namespace tracker