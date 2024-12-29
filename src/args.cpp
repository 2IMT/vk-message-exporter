#include "args.h"

#include <optional>
#include <format>

namespace vme
{

    static std::optional<std::string> _adv_args(int* argc, char*** argv)
    {
        if (*argc == 0)
        {
            return std::nullopt;
        }

        std::string arg = **argv;
        (*argc)--;
        (*argv)++;

        return arg;
    }

    args::args(int argc, char** argv) :
        m_program_name(_adv_args(&argc, &argv).value())
    {
        bool access_token_set = false;
        bool peer_id_set = false;

        std::string access_token;
        std::string peer_id;
        std::string storage_root = "./";
        std::string show_progress = "true";

        while (true)
        {
            std::optional<std::string> arg = _adv_args(&argc, &argv);
            if (!arg.has_value())
            {
                break;
            }

            std::optional<std::string> value = _adv_args(&argc, &argv);
            if (!value.has_value())
            {
                throw args_parse_error(
                    std::format("Invalid argument: value for argument \"{}\" "
                                "not provided\n{}",
                        arg.value(), help()));
            }

            if (arg.value() == ACCESS_TOKEN_ARG ||
                arg.value() == ACCESS_TOKEN_ARG_SHORT)
            {
                access_token = value.value();
                access_token_set = true;
            }
            else if (arg.value() == PEER_ID_ARG ||
                     arg.value() == PEER_ID_ARG_SHORT)
            {
                peer_id = value.value();
                peer_id_set = true;
            }
            else if (arg.value() == STORAGE_ROOT_ARG ||
                     arg.value() == STORAGE_ROOT_ARG_SHORT)
            {
                storage_root = value.value();
            }
            else if (arg.value() == SHOW_PROGRESS_ARG ||
                     arg.value() == SHOW_PROGRESS_ARG_SHORT)
            {
                show_progress = value.value();
            }
            else
            {
                throw args_parse_error(std::format(
                    "Invalid argument: \"{}\"\n{}", arg.value(), help()));
            }
        }

        if (!access_token_set && !peer_id_set)
        {
            throw args_parse_error(std::format(
                "Invalid argument: access_token and peer_id not set\n{}",
                help()));
        }

        if (!access_token_set)
        {
            throw args_parse_error(std::format(
                "Invalid argument: access_token not set\n{}", help()));
        }

        if (!peer_id_set)
        {
            throw args_parse_error(
                std::format("Invalid argument: peer_id not set\n{}", help()));
        }

        std::int64_t peer_id_parsed;
        try
        {
            peer_id_parsed = std::stoll(peer_id);
        }
        catch (const std::out_of_range&)
        {
            throw args_parse_error(std::format(
                "Invalid argument: peer_id is not an integer\n{}", help()));
        }
        catch (const std::invalid_argument&)
        {
            throw args_parse_error(std::format(
                "Invalid argument: peer_id is out of int64 bounds\n{}",
                help()));
        }

        bool show_progress_parsed;
        if (show_progress == "true")
        {
            show_progress_parsed = true;
        }
        else if (show_progress == "false")
        {
            show_progress_parsed = false;
        }
        else
        {
            throw args_parse_error(std::format(
                "Invalid argument: show_progress is not a boolean\n{}",
                help()));
        }

        m_access_token = access_token;
        m_peer_id = peer_id_parsed;
        m_storage_root = storage_root;
        m_show_progress = show_progress_parsed;
    }

    std::string args::program_name() const noexcept { return m_program_name; }

    std::string args::access_token() const noexcept { return m_access_token; }

    std::int64_t args::peer_id() const noexcept { return m_peer_id; }

    std::string args::storage_root() const noexcept { return m_storage_root; }

    bool args::show_progress() const noexcept { return m_show_progress; }

    std::string args::help() const noexcept
    {
        return std::format(
            R""""(Usage: {} -access_token <value> -peer_id <value> [-storage_root <value>] [-show_progress <true|false>]
-access_token  -t (string)  REQUIRED :
    Sets access token for VK API.
-peer_id       -p (integer) REQUIRED :
    Sets peer id to export message history with.
-storage_root  -r (string)  OPTIONAL :
    Sets directory to store data in.
    (Default: current working directory)
-show_progress -s (boolean) OPTIONAL :
    Shows progress when set to true
    (Default: true))"""",
            m_program_name);
    }

}
