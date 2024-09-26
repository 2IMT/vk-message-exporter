#include "session.h"

#include <cstdlib>
#include <format>
#include <utility>
#include <sstream>

#include <curl/curl.h>

namespace vme::api
{

    static void _curl_cleanup() noexcept { curl_global_cleanup(); }

    static size_t _curl_write_data(
        void* buffer, size_t size, size_t nmemb, void* user_data) noexcept
    {
        std::stringstream* data_stream =
            reinterpret_cast<std::stringstream*>(user_data);
        std::string chunk(reinterpret_cast<char*>(buffer), nmemb);
        *data_stream << chunk;
        return nmemb;
    }

    session::session(const std::string& host) :
        m_host(host),
        m_curl(nullptr)
    {
        if (!curl_initialized)
        {
            CURLcode status = curl_global_init(CURL_GLOBAL_ALL);
            if (status != 0)
            {
                throw session_init_error(
                    std::format("Failed to initialize CURL: {}",
                        curl_easy_strerror(status)));
            }

            std::atexit(_curl_cleanup);
            curl_initialized = true;
        }

        m_curl = curl_easy_init();
        if (m_curl == nullptr)
        {
            throw session_init_error("Failed to create CURL handle");
        }
    }

    session::session(const session& other) noexcept :
        m_host(other.m_host),
        m_curl(curl_easy_duphandle(other.m_curl))
    {
    }

    session::session(session&& other) noexcept :
        m_host(std::move(other.m_host)),
        m_curl(other.m_curl)
    {
        other.m_curl = nullptr;
    }

    session& session::operator=(const session& other) noexcept
    {
        m_host = other.m_host;

        curl_easy_cleanup(m_curl);
        m_curl = curl_easy_duphandle(other.m_curl);

        return *this;
    }

    session& session::operator=(session&& other) noexcept
    {
        m_host = std::move(other.m_host);

        curl_easy_cleanup(m_curl);
        m_curl = other.m_curl;
        other.m_curl = nullptr;

        return *this;
    }

    session::~session() noexcept
    {
        if (m_curl != nullptr)
        {
            curl_easy_cleanup(m_curl);
        }
    }

    std::string session::call(const std::string& method, params p)
    {
        std::stringstream addr_stream;
        addr_stream << "https://" << m_host << "/" << method << "?"
                    << p.to_query();
        std::string addr = addr_stream.str();

        std::stringstream data_stream;

        curl_easy_setopt(m_curl, CURLOPT_URL, addr.c_str());
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, _curl_write_data);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data_stream);

        CURLcode status = curl_easy_perform(m_curl);

        if (status != 0)
        {
            throw session_call_error(std::format(
                "Failed to call \"{}\": {}", addr, curl_easy_strerror(status)));
        }

        return data_stream.str();
    }

    bool session::curl_initialized = false;

}
