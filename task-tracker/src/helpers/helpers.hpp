#pragma once

#include <regex>
#include <string_view>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>

namespace helpers {

bool IsEmailValid(const std::string_view& email);

std::string FillEmptyResponseWithStatus(const userver::server::http::HttpRequest& request, userver::server::http::HttpStatus status);

std::optional<int> GetIntValueByKey(const userver::server::http::HttpRequest& request, const std::string& key);

}  // namespace helpers
