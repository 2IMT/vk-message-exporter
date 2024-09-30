#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "error.h"

namespace vme::api
{

    class json_response_parse_error : public error
    {
    public:
        json_response_parse_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class json_response_error : public error
    {
    public:
        json_response_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class json_response
    {
    public:
        json_response(const std::string& response_text);

        nlohmann::json& get() noexcept;

    private:
        nlohmann::json m_json;
    };

}
