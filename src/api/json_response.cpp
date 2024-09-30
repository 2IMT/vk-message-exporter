#include "json_response.h"

#include <format>

namespace vme::api
{

    json_response::json_response(const std::string& response_text)
    {
        try
        {
            nlohmann::json response_json = nlohmann::json::parse(response_text);

            if (response_json.contains("error"))
            {
                nlohmann::json& error_object = response_json.at("error");

                nlohmann::json& error_code_obj = error_object.at("error_code");
                nlohmann::json& error_msg_obj = error_object.at("error_msg");

                throw json_response_error(
                    std::format("API returned an error: {} {}",
                        error_code_obj.template get<int>(),
                        error_msg_obj.template get<std::string>()));
            }

            m_json = response_json.at("response");
        }
        catch (const nlohmann::json::exception& e)
        {
            throw json_response_parse_error(
                std::format("Invalid API response: {}", e.what()));
        }
    }

    nlohmann::json& json_response::get() noexcept { return m_json; }

}
