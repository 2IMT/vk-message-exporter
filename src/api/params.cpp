#include "params.h"

#include <sstream>
#include <cstddef>

namespace vme::api
{

    std::string param::key() const noexcept { return m_key; }

    std::string param::value() const noexcept { return m_value; }

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
