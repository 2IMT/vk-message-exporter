#pragma once

#include <unordered_map>
#include <cstdint>
#include <string>

#include "session.h"
#include "vk_data.h"
#include "error.h"

namespace vme::api
{

    class user_pool_response_error : public error
    {
    public:
        user_pool_response_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class user_pool
    {
    public:
        user_pool(session& s, const std::string& access_token);

        auto begin() const { return m_users.begin(); }

        auto end() const { return m_users.end(); }

        void pull_users(const vk_data::message& message);

    private:
        session& m_session;
        std::string m_access_token;
        std::unordered_map<std::int64_t, vk_data::user> m_users;
    };

}
