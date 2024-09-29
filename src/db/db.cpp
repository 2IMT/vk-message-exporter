#include "db.h"

#include <format>

#include <sqlite3.h>

#include "sql.h"

namespace vme::db
{

    db::db(const std::string& db_file_path)
    {
        int status = sqlite3_open(db_file_path.c_str(), &m_sqlite3);
        if (status)
        {
            throw db_init_error(
                std::format("Failed to open database \"{}\": {}", db_file_path,
                    sqlite3_errmsg(m_sqlite3)));
        }

        char* message = nullptr;
        status = sqlite3_exec(m_sqlite3, sql::init.c_str(), nullptr, nullptr, &message);
        if (status)
        {
            throw db_init_error(
                std::format("Failed to initialize database \"{}\": {}",
                    db_file_path, message));
            sqlite3_free(message);
        }
    }

    db::db(db&& other) noexcept :
        m_sqlite3(other.m_sqlite3)
    {
        other.m_sqlite3 = nullptr;
    }

    db& db::operator=(db&& other) noexcept
    {
        sqlite3_close(m_sqlite3);
        m_sqlite3 = other.m_sqlite3;
        other.m_sqlite3 = nullptr;

        return *this;
    }

    db::~db() noexcept { sqlite3_close(m_sqlite3); }

}
