#pragma once

#include <string>

#include "error.h"

struct sqlite3;
typedef struct sqlite3 sqlite3;

namespace vme::db
{

    class db_init_error : public error
    {
    public:
        db_init_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class db
    {
    public:
        db(const std::string& db_file_path);
        ~db() noexcept;

    private:
        sqlite3* m_sqlite3;
    };

}
