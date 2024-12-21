#pragma once
#include "user.hpp"
#include <userver/chaotic/exception.hpp>
#include <userver/chaotic/object.hpp>
#include <userver/chaotic/primitive.hpp>
#include <userver/chaotic/validators.hpp>
#include <userver/chaotic/with_type.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/utils/trivial_map.hpp>

namespace tracker {

static constexpr userver::utils::TrivialSet ktracker__TUser_PropertiesNames = [](auto selector) {
    return selector().template Type<std::string_view>().Case("id").Case("login").Case("password").Case("created_date").Case("deleted_date");
};

template<typename Value>
tracker::TUser Parse(Value value, userver::formats::parse::To<tracker::TUser>) {
    value.CheckNotMissing();
    value.CheckObjectOrNull();
    tracker::TUser res;
    res.id = value["id"].template As<userver::chaotic::Primitive<int>>();
    res.login = value["login"].template As<userver::chaotic::Primitive<std::string>>();
    res.password = value["password"].template As<userver::chaotic::Primitive<std::string>>();
    res.created_date = value["created_date"].template As<userver::chaotic::WithType<userver::chaotic::Primitive<std::string>, userver::utils::datetime::TimePointTz>>();
    res.deleted_date = value["deleted_date"].template As<std::optional<userver::chaotic::WithType<userver::chaotic::Primitive<std::string>, userver::utils::datetime::TimePointTz>>>();
    userver::chaotic::ValidateNoAdditionalProperties(value, ktracker__TUser_PropertiesNames);
    return res;
}

}