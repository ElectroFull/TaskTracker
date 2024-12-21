#include "helpers.hpp"

namespace helpers {

//from https://stackoverflow.com/questions/56907206/using-regex-for-email-input-validation-c
bool IsEmailValid(const std::string_view& email) {
    // define a regular expression
    const std::regex pattern
    (R"([_a-z0-9-]+(\.[_a-z0-9-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4}))");

    // try to match the string with the regular expression
    return std::regex_match(email.begin(), email.end(), pattern);
}

std::string FillEmptyResponseWithStatus(const userver::server::http::HttpRequest& request, userver::server::http::HttpStatus status) {
    auto& response = request.GetHttpResponse();
    response.SetStatus(status);
    return {};
}

std::optional<int> GetIntValueByKey(const userver::server::http::HttpRequest& request, const std::string& key) {
    try {
        return stoi(request.GetPathArg(key));
    } catch (...) {
        return std::nullopt;
    }
}

}  // namespace helpers
