#include "params.h"

#include <sstream>
#include <cstddef>
#include <format>

namespace vme::api
{

    static void _throw_param_not_found(const std::string& name)
    {
        throw param_not_found_error(
            std::format("Param \"{}\" not found", name));
    }

    static void _throw_param_index_out_of_bounds(
        std::size_t index, std::size_t size)
    {
        throw param_index_out_of_bounds(
            std::format("Index {} is out of bounds (size: {})", index, size));
    }

    std::string param::key() const noexcept { return m_key; }

    std::string param::value() const noexcept { return m_value; }

    void params::set_param(const param& p)
    {
        bool found = false;
        for (std::size_t i = 0; i < m_values.size(); i++)
        {
            if (m_values[i].key() == p.key())
            {
                found = true;
                m_values[i] = p;
            }
        }

        if (!found)
        {
            _throw_param_not_found(p.key());
        }
    }

    void params::set_param(std::size_t index, const param& p)
    {
        if (index >= m_values.size())
        {
            _throw_param_index_out_of_bounds(index, m_values.size());
        }

        m_values[index] = p;
    }

    void params::add_param(const param& p) noexcept { m_values.push_back(p); }

    void params::remove_param(const std::string& key)
    {
        std::vector<param> new_values;
        new_values.reserve(m_values.size());

        bool found = false;
        for (std::size_t i = 0; i < m_values.size(); i++)
        {
            if (m_values[i].key() != key)
            {
                new_values.push_back(m_values[i]);
            }
            else
            {
                found = true;
            }
        }

        if (!found)
        {
            _throw_param_not_found(key);
        }

        m_values = new_values;
    }

    void params::remove_param(std::size_t index)
    {
        if (index >= m_values.size())
        {
            _throw_param_index_out_of_bounds(index, m_values.size());
        }

        m_values.erase(m_values.begin() + index);
    }

    std::string params::to_query() const noexcept
    {
        std::stringstream query_stream;

        for (std::size_t i = 0; i < m_values.size(); i++)
        {
            query_stream << m_values[i].key() << "=" << m_values[i].value();

            if (i != m_values.size() - 1)
            {
                query_stream << "&";
            }
        }

        return query_stream.str();
    }

}
