#include <iostream>
#include <cstddef>

#include "error.h"
#include "args.h"
#include "api/session.h"
#include "api/message_stream.h"
#include "api/vk_data.h"

int main(int argc, char** argv)
{
    try
    {
        vme::args args(argc, argv);
        vme::api::session session("api.vk.com");

        vme::api::message_stream message_stream(
            std::move(session), args.peer_id(), args.access_token());

        for (std::size_t i = 0; i < 2100; i++)
        {
            std::optional<vme::api::vk_data::message> message =
                message_stream.next();
            if (!message.has_value())
            {
                break;
            }

            std::cout << "Message-----------------------------------"
                      << std::endl;
            std::cout << message.value().from_id << std::endl;
            std::cout << message.value().conversation_message_id << std::endl;
            std::cout << message.value().date << std::endl;
            std::cout << message.value().text << std::endl;
        }
    }
    catch (const vme::error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
