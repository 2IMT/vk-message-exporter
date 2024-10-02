#include "curl.h"

#include <cstdlib>
#include <cstddef>
#include <format>

#include <curl/curl.h>

namespace vme
{

    static size_t _curl_write_function(void* buffer, std::size_t size,
        std::size_t nmemb, void* user_data) noexcept
    {
        std::ostream& data_stream = *reinterpret_cast<std::ostream*>(user_data);
        data_stream.write(reinterpret_cast<char*>(buffer), nmemb);
        return nmemb;
    }

    curl::curl() :
        m_curl(nullptr)
    {
        if (!curl_initialized)
        {
            CURLcode status = curl_global_init(CURL_GLOBAL_ALL);
            if (status != CURLE_OK)
            {
                throw curl_init_error(
                    std::format("Failed to initialize CURL: {}",
                        curl_easy_strerror(status)));
            }

            std::atexit(curl_global_cleanup);
            curl_initialized = true;
        }

        m_curl = curl_easy_init();
        if (m_curl == nullptr)
        {
            throw curl_init_error("Failed to create CURL handle");
        }
    }

    curl::curl(const curl& other) noexcept :
        m_curl(curl_easy_duphandle(other.m_curl))
    {
    }

    curl::curl(curl&& other) noexcept :
        m_curl(other.m_curl)
    {
        other.m_curl = nullptr;
    }

    curl& curl::operator=(const curl& other) noexcept
    {
        curl_easy_cleanup(m_curl);
        m_curl = curl_easy_duphandle(other.m_curl);

        return *this;
    }

    curl& curl::operator=(curl&& other) noexcept
    {
        curl_easy_cleanup(m_curl);
        m_curl = other.m_curl;
        other.m_curl = nullptr;

        return *this;
    }

    curl::~curl() noexcept
    {
        if (m_curl != nullptr)
        {
            curl_easy_cleanup(m_curl);
        }
    }

    void curl::perform(const std::string& url, std::ostream& ostream)
    {
        curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, _curl_write_function);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &ostream);

        CURLcode status = curl_easy_perform(m_curl);
        if (status != CURLE_OK)
        {
            throw curl_perform_error(
                std::format("Failed to perform on URL \"{}\": {}", url,
                    curl_easy_strerror(status)));
        }
    }

    bool curl::curl_initialized = false;

}
