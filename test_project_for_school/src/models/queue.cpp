#include <userver/chaotic/type_bundle_cpp.hpp>
#include "queue.hpp"
#include "queue.ipp"

namespace tracker
{
bool operator==(const tracker::TQueue& lhs, const tracker::TQueue& rhs) {
    return lhs.id == rhs.id
        && lhs.name == rhs.name
        && lhs.author_id == rhs.author_id
        && lhs.tickets_count == rhs.tickets_count
        && lhs.created_date == rhs.created_date
        && lhs.deleted_date == rhs.deleted_date
        && true;
}

userver::logging::LogHelper& operator<<(userver::logging::LogHelper& lh, const tracker::TQueue& value) {
    return lh << ToString(userver::formats::json::ValueBuilder(value).ExtractValue());
}

TQueue Parse(userver::formats::json::Value json, userver::formats::parse::To<tracker::TQueue> to) {
    return Parse<userver::formats::json::Value>(json, to);
}

userver::formats::json::Value Serialize([[maybe_unused]] const tracker::TQueue& value, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder vb = userver::formats::common::Type::kObject;

    vb["id"] = userver::chaotic::Primitive<int>{value.id};
    vb["name"] = userver::chaotic::Primitive<std::string>{value.name};

    vb["author_id"] = userver::chaotic::Primitive<int>{value.author_id};

    vb["tickets_count"] = userver::chaotic::Primitive<int>{ value.tickets_count };

    vb["created_date"] = value.created_date;

    if (value.deleted_date) {
        vb["deleted_date"] = *value.deleted_date;
    }

    return vb.ExtractValue();
}
}