#pragma once

#include <optional>
#include <cstdint>
#include <string>
#include <cstddef>
#include <queue>

#include "api/session.h"
#include "vk_data.h"
#include "error.h"

namespace vme::api
{

    class message_stream_invalid_response_error : public error
    {
    public:
        message_stream_invalid_response_error(const std::string& message) :
            error(message)
        {
        }
    };

    class message_stream_api_error : public error
    {
    public:
        message_stream_api_error(const std::string& message) :
            error(message)
        {
        }
    };

    class message_stream
    {
    public:
        message_stream(api::session&& s, std::int64_t peer_id,
            std::string access_token) noexcept;

        std::optional<vk_data::message> next();

        std::size_t message_count() const noexcept;

    private:
        api::session m_session;
        std::int64_t m_peer_id;
        std::string m_access_token;
        std::size_t m_current_offset;
        std::queue<vk_data::message> m_message_buffer;
        std::size_t m_message_count;
    };

}
