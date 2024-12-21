#include <userver/chaotic/type_bundle_cpp.hpp>
#include "user.hpp"
#include "user.ipp"

namespace tracker {

TUser Parse(userver::formats::json::Value json, userver::formats::parse::To<tracker::TUser> to) {
    return Parse<userver::formats::json::Value>(json, to);
}

userver::formats::json::Value Serialize(const tracker::TUser& user, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder vb = userver::formats::common::Type::kObject;

    vb["id"] = user.id;
    vb["login"] = user.login;

    return vb.ExtractValue();
}

}