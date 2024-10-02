#include "session.h"

#include <cstdlib>
#include <sstream>

#include <curl/curl.h>

namespace vme::api
{

    session::session(const std::string& host) :
        m_host(host)
    {
    }

    std::string session::call(const std::string& method, params p)
    {
        std::stringstream addr_stream;
        addr_stream << "https://" << m_host << "/" << method << "?"
                    << p.to_query();

        std::stringstream data_stream;

        m_curl.perform(addr_stream.str(), data_stream);

        return data_stream.str();
    }

}
