#pragma once

#include <string>
#include <exception>

namespace vme
{

    class error : public std::exception
    {
    public:
        error(const std::string& message) noexcept :
            m_message(message)
        {
        }

        const char* what() const noexcept override { return m_message.c_str(); }

    private:
        std::string m_message;
    };

}
