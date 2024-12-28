#include <userver/chaotic/type_bundle_cpp.hpp>
#include "ticket.hpp"
#include "ticket.ipp"



namespace tracker
{

    bool operator==(const tracker::TTicket& lhs, const tracker::TTicket& rhs) {
        return lhs.id == rhs.id
            && lhs.title == rhs.title
            && lhs.description == rhs.description
            && lhs.author_id == rhs.author_id
            && lhs.performer_id == rhs.performer_id
            && lhs.priority == rhs.priority
            && lhs.queue_id == rhs.queue_id
            && lhs.status_id == rhs.status_id
            && lhs.start_date == rhs.start_date
            && lhs.finish_date == rhs.finish_date
            && lhs.deadline_date == rhs.deadline_date
            && lhs.created_date == rhs.created_date
            && lhs.deleted_date == rhs.deleted_date
            && true;
    }

    userver::logging::LogHelper& operator<<(userver::logging::LogHelper& lh, const tracker::TTicket& value) {
        return lh << ToString(userver::formats::json::ValueBuilder(value).ExtractValue());
    }

    userver::formats::json::Value Serialize([[maybe_unused]] const tracker::TTicket& value, userver::formats::serialize::To<userver::formats::json::Value>) {
        userver::formats::json::ValueBuilder vb = userver::formats::common::Type::kObject;

        vb["id"] = userver::chaotic::Primitive<std::string>{value.id};
        vb["title"] = userver::chaotic::Primitive<std::string>{value.title};

        if (value.description) {
            vb["description"] = userver::chaotic::Primitive<std::string>{*value.description};
        }

        vb["author_id"] = userver::chaotic::Primitive<int>{value.author_id};

        if (value.performer_id) {
            vb["performer_id"] = userver::chaotic::Primitive<int>{*value.performer_id};
        }

        vb["priority"] = ToString(value.priority);
        vb["queue_id"] = userver::chaotic::Primitive<int>{value.queue_id};
        vb["status_id"] = userver::chaotic::Primitive<int>{value.status_id};

        if (value.start_date) {
            vb["start_date"] = TimePoint{*value.start_date};
        }

        if (value.finish_date) {
            vb["finish_date"] = TimePoint{*value.finish_date};
        }

        if (value.deadline_date) {
            vb["deadline_date"] = TimePoint{*value.deadline_date};
        }

        if (value.created_date) {
            vb["created_date"] = TimePoint{*value.created_date};
        }

        if (value.deleted_date) {
            vb["deleted_date"] = TimePoint{*value.deleted_date};
        }

        return vb.ExtractValue();
    }

    std::string ToString(tracker::TTicket::Priority value) {
        const auto result = ktracker__TTicket__Priority_Mapping.TryFindByFirst(value);
        if (result.has_value()) {
            return std::string{*result};
        }
        throw std::runtime_error("Bad enum value");
    }

    TTicket::Priority Parse(const userver::formats::json::Value &val, userver::formats::parse::To<TTicket::Priority>) {
        const auto value = val.template As<std::string>();
        const auto result = ktracker__TTicket__Priority_Mapping.TryFindBySecond(value);
        if (result.has_value()) {
            return *result;
        }
        userver::chaotic::ThrowForValue(fmt::format("Invalid enum value ({}) for type tracker::TTicket::Priority", value), val);
    }  

    TTicket Parse(const userver::formats::json::Value &value, userver::formats::parse::To<TTicket>) {
        value.CheckNotMissing();
        value.CheckObjectOrNull();
        tracker::TTicket res;
        res.id = value["id"].As<std::string>(); 
        res.title = value["title"].As<std::string>(); 
        res.description = value["description"].As<std::string>(); 
        res.author_id = value["author_id"].As<int>(); 
        res.performer_id = value["performer_id"].As<int>();
        res.priority = value["priority"].As<tracker::TTicket::Priority>(); 
        res.queue_id = value["queue_id"].As<int>();
        res.status_id = value["status_id"].As<int>();
        res.start_date = value["start_date"].As<std::optional<TimePoint>>(); 
        res.finish_date = value["finish_date"].As<std::optional<TimePoint>>();  
        res.deadline_date = value["deadline_date"].As<std::optional<TimePoint>>();  
        res.created_date = value["created_date"].As<std::optional<TimePoint>>();  
        res.deleted_date = value["deleted_date"].As<std::optional<TimePoint>>(); 
        return res;
    }

} // namespace tracker