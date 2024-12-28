#include "user_queue.hpp"

namespace tracker {

userver::formats::json::Value Serialize(const tracker::TUserQueue& userQueue, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder vb = userver::formats::common::Type::kObject;

    vb["user_id"] = userQueue.user_id;
    vb["queue_id"] = userQueue.queue_id;

    return vb.ExtractValue();
}

}