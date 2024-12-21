#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

namespace tracker {

void AppendGetQueues(userver::components::ComponentList& component_list);

}  // namespace tracker
