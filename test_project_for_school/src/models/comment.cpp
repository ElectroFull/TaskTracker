#include <userver/chaotic/type_bundle_cpp.hpp>
#include "comment.hpp"
#include "comment.ipp"

namespace tracker
{

bool operator==(const tracker::TComment& lhs, const tracker::TComment& rhs) {
    return lhs.id == rhs.id
        && lhs.message == rhs.message
        && lhs.ticket_id == rhs.ticket_id
        && lhs.created_date == rhs.created_date
        && lhs.deleted_date == rhs.deleted_date
        && true;
}

userver::logging::LogHelper& operator<<(userver::logging::LogHelper& lh, const tracker::TComment& value) {
    return lh << ToString(userver::formats::json::ValueBuilder(value).ExtractValue());
}

TComment Parse(userver::formats::json::Value json, userver::formats::parse::To<tracker::TComment> to) {
    return Parse<userver::formats::json::Value>(json, to);
}

userver::formats::json::Value Serialize([[maybe_unused]] const tracker::TComment& value, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder vb = userver::formats::common::Type::kObject;

    vb["id"] = userver::chaotic::Primitive<int>{value.id};
    vb["message"] = userver::chaotic::Primitive<std::string>{value.message};
    vb["ticket_id"] = userver::chaotic::Primitive<std::string>{value.ticket_id};

    vb["created_date"] = value.created_date;

    if (value.deleted_date) {
        vb["deleted_date"] = *value.deleted_date;
    }

    return vb.ExtractValue();
}

} // namespace tracker