#pragma once

#include <string>
#include <ostream>

#include "error.h"

namespace vme
{

    class curl_init_error : public error
    {
    public:
        curl_init_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class curl_perform_error : public error
    {
    public:
        curl_perform_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class curl
    {
    public:
        curl();

        curl(const curl& other) noexcept;

        curl(curl&& other) noexcept;

        curl& operator=(const curl& other) noexcept;

        curl& operator=(curl&& other) noexcept;

        ~curl() noexcept;

        void perform(const std::string& url, std::ostream& ostream);

    private:
        static bool curl_initialized;

        void* m_curl;
    };

}
