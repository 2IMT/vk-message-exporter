#include <iostream>
#include <iomanip>
#include <cstddef>
#include <exception>
#include <format>
#include <filesystem>

#include "error.h"
#include "args.h"
#include "api/session.h"
#include "api/message_stream.h"
#include "api/vk_data.h"
#include "api/user_pool.h"
#include "db/db.h"
#include "db/storage.h"

int main(int argc, char** argv)
{
    try
    {
        vme::args args(argc, argv);

        try
        {
            if (std::filesystem::exists(args.storage_root()))
            {
                if (!std::filesystem::is_directory(args.storage_root()))
                {
                    throw vme::error(std::format(
                        "Storage root \"{}\" exists and is not a directory",
                        args.storage_root()));
                }
            }
            else
            {
                std::filesystem::create_directory(args.storage_root());
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            throw vme::error(
                std::format("Failed to ensure storage root \"{}\" exists: {}",
                    args.storage_root(), e.what()));
        }

        vme::api::session session("api.vk.com");
        vme::api::message_stream message_stream(
            session, args.peer_id(), args.access_token());
        vme::api::user_pool user_pool(session, args.access_token());
        vme::db::db db(std::format("{}/database.db", args.storage_root()));
        vme::db::storage storage(args.storage_root(), db);

        std::cout << std::fixed << std::setprecision(2);

        for (std::size_t i = 0;; i++)
        {
            std::optional<vme::api::vk_data::message> message =
                message_stream.next();
            if (!message.has_value())
            {
                break;
            }

            storage.put(message.value());
            user_pool.pull_users(message.value());

            if (args.show_progress())
            {
                std::size_t message_count = message_stream.message_count();
                std::cout << "Processed " << i << "/" << message_count
                          << " messages ("
                          << static_cast<float>(i) /
                                 static_cast<float>(message_count) * 100
                          << "%)" << std::endl;
            }
        }

        db.put(user_pool);
        storage.download_media(args.show_progress());
    }
    catch (const vme::error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unknown error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
