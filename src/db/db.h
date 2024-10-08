#pragma once

#include <string>

#include "error.h"
#include "api/vk_data.h"
#include "api/user_pool.h"

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

    class db_operation_error : public error
    {
    public:
        db_operation_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class db
    {
    public:
        db(const std::string& db_file_path);

        db(const db& other) = delete;

        db(db&& other) noexcept;

        db& operator=(const db& other) = delete;

        db& operator=(db&& other) noexcept;

        ~db() noexcept;

        void put(const api::vk_data::message& message);

        void put(const api::user_pool& user_pool);

    private:
        sqlite3* m_sqlite3;
    };

}
