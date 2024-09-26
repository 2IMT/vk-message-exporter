#pragma once

#include <string>

#include "error.h"
#include "params.h"

namespace vme::api
{

    class session_init_error : public error
    {
    public:
        session_init_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class session_call_error : public error
    {
    public:
        session_call_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class session
    {
    public:
        session(const std::string& host);

        session(const session& other) noexcept;

        session(session&& other) noexcept;

        session& operator=(const session& other) noexcept;

        session& operator=(session&& other) noexcept;

        ~session() noexcept;

        std::string call(const std::string& method, params p);

    private:
        std::string m_host;
        void* m_curl;

        static bool curl_initialized;
    };

}
