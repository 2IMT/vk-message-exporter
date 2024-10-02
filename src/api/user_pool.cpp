#include "user_pool.h"
#include "api/vk_data.h"

#include <unordered_set>
#include <vector>
#include <sstream>
#include <cstddef>
#include <cstdint>

#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>

namespace vme::api
{

    static vk_data::user _parse_user(const nlohmann::json& user_item)
    {
        vk_data::user result;
        result.id = user_item.at("id").template get<std::int64_t>();
        result.first_name =
            user_item.at("first_name").template get<std::string>();
        result.last_name =
            user_item.at("last_name").template get<std::string>();
        return result;
    }

    static std::vector<vk_data::user> _parse_users(const std::string& response)
    {
        try
        {
            nlohmann::json response_json = nlohmann::json::parse(response);

            if (response_json.contains("error"))
            {
                nlohmann::json& error_object = response_json.at("error");

                nlohmann::json& error_code_obj = error_object.at("error_code");
                nlohmann::json& error_msg_obj = error_object.at("error_msg");

                throw user_pool_response_error(
                    std::format("API returned an error: {} {}",
                        error_code_obj.template get<int>(),
                        error_msg_obj.template get<std::string>()));
            }

            nlohmann::json body = response_json.at("response");

            std::vector<vk_data::user> result;
            result.reserve(body.size());

            for (const auto& user_item : body)
            {
                result.push_back(_parse_user(user_item));
            }

            return result;
        }
        catch (const nlohmann::json::exception& e)
        {
            throw user_pool_response_error(
                std::format("Invalid API response: {}", e.what()));
        }
    }

    static void _collect_ids(
        const vk_data::message& message, std::unordered_set<std::int64_t>& ids)
    {
        ids.insert(message.from_id);

        for (const auto& attachment : message.attachments)
        {
            switch (attachment.type)
            {
                using enum vk_data::attachment_type;
            case photo:
                ids.insert(attachment.photo_value.owner_id);
                break;

            case video:
                ids.insert(attachment.video_value.owner_id);
                break;

            case audio:
                ids.insert(attachment.audio_value.owner_id);
                break;

            case document:
                ids.insert(attachment.document_value.owner_id);
                break;

            case product:
                ids.insert(attachment.product_value.owner_id);
                break;

            case product_album:
                ids.insert(attachment.product_album_value.owner_id);
                break;

            case post:
                ids.insert(attachment.post_value.from_id);
                ids.insert(attachment.post_value.owner_id);
                break;

            case comment:
                ids.insert(attachment.comment_value.from_id);
                break;

            case call:
                ids.insert(attachment.call_value.initiator_id);
                ids.insert(attachment.call_value.receiver_id);
                break;

            case audio_message:
                ids.insert(attachment.audio_message_value.owner_id);
                break;

            case audio_playlist:
                ids.insert(attachment.audio_playlist_value.owner_id);
                for (const auto& audio : attachment.audio_playlist_value.audios)
                {
                    ids.insert(audio.owner_id);
                }
                break;

            case graffiti:
                ids.insert(attachment.graffiti_value.owner_id);
                break;

            case money_request:
                ids.insert(attachment.money_request_value.from_id);
                ids.insert(attachment.money_request_value.to_id);
                break;

            case story:
                ids.insert(attachment.story_value.owner_id);
                break;

            default:
                break;
            }
        }

        for (const auto& fwd_message : message.fwd_messages)
        {
            _collect_ids(*fwd_message.get(), ids);
        }
    }

    user_pool::user_pool(session& s, const std::string& access_token) :
        m_session(s),
        m_access_token(access_token)
    {
    }

    void user_pool::pull_users(const vk_data::message& message)
    {
        std::unordered_set<std::int64_t> ids;
        _collect_ids(message, ids);

        std::vector<std::int64_t> queried_ids;
        queried_ids.reserve(32);
        for (auto id : ids)
        {
            if (!m_users.contains(id))
            {
                queried_ids.push_back(id);
            }
        }

        if (queried_ids.size() == 0)
        {
            return;
        }

        std::stringstream id_list;
        for (std::size_t i = 0; i < queried_ids.size(); i++)
        {
            id_list << queried_ids[i];
            if (i != queried_ids.size() - 1)
            {
                id_list << ",";
            }
        }

        // clang-format off
        std::string response = 
            m_session.call("method/users.get", {
                { "user_ids",     id_list.str() },
                { "access_token", m_access_token },
                { "v",            "5.199" }
        });
        // clang-format on

        std::vector<vk_data::user> users = _parse_users(response);

        for (const auto& user : users)
        {
            m_users[user.id] = user;
        }
    }

}
