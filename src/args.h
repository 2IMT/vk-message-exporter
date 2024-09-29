#pragma once

#include <cstdint>
#include <string>

#include "error.h"

namespace vme
{

    class args_parse_error : public error
    {
    public:
        args_parse_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class args
    {
    public:
        args(int argc, char** argv);

        std::string program_name() const noexcept;

        std::string access_token() const noexcept;

        std::int64_t peer_id() const noexcept;

        std::string storage_root() const noexcept;

        bool show_progress() const noexcept;

        std::string help() const noexcept;

    private:
        static inline const std::string ACCESS_TOKEN_ARG = "-access_token";
        static inline const std::string ACCESS_TOKEN_ARG_SHORT = "-t";
        static inline const std::string PEER_ID_ARG = "-peer_id";
        static inline const std::string PEER_ID_ARG_SHORT = "-p";
        static inline const std::string STORAGE_ROOT_ARG = "-storage_root";
        static inline const std::string STORAGE_ROOT_ARG_SHORT = "-r";
        static inline const std::string SHOW_PROGRESS_ARG = "-show_progress";
        static inline const std::string SHOW_PROGRESS_ARG_SHORT = "-s";

        std::string m_program_name;
        std::string m_access_token;
        std::int64_t m_peer_id;
        std::string m_storage_root;
        bool m_show_progress;
    };

}
