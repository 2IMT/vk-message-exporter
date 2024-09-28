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

        m_access_token = access_token;
        m_peer_id = peer_id_parsed;
    }

    std::string args::program_name() const noexcept { return m_program_name; }

    std::string args::access_token() const noexcept { return m_access_token; }

    std::int64_t args::peer_id() const noexcept { return m_peer_id; }

    std::string args::help() const noexcept
    {
        return std::format(
            R""""(Usage: {} -access_token <value> -peer_id <value>
-access_token    -t    (string)     sets access token for VK API.
-peer_id         -p    (integer)    sets peer id to export message history with.)"""",
            m_program_name);
    }

}
