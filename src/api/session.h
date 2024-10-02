#pragma once

#include <string>

#include "params.h"
#include "curl.h"

namespace vme::api
{

    class session
    {
    public:
        session(const std::string& host);

        std::string call(const std::string& method, params p);

    private:
        std::string m_host;
        curl m_curl;
    };

}
