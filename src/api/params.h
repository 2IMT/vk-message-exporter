#pragma once

#include <string>
#include <format>
#include <vector>
#include <type_traits>

namespace vme::api
{

    class param
    {
    public:
        template<class T>
        param(const std::string& key, T&& value) noexcept :
            m_key(key),
            m_value(std::format("{}", std::forward<T>(value)))
        {
        }

        std::string key() const noexcept;

        std::string value() const noexcept;

    private:
        std::string m_key;
        std::string m_value;
    };

    class params
    {
    public:
        params(std::initializer_list<param> list) noexcept :
            m_values(list)
        {
        }

        template<class... Args>
        params(Args&&... values) noexcept
        {
            static_assert(std::conjunction_v<std::is_same<Args, param>...>,
                "values should be instances of vme::param");
            (m_values.emplace_back(std::forward<Args>(values)), ...);
        }

        auto begin() const noexcept { return m_values.begin(); }

        auto end() const noexcept { return m_values.end(); }

        std::string to_query() const noexcept;

    private:
        std::vector<param> m_values;
    };

}
