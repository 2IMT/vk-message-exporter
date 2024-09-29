#include "message_stream.h"
#include "vk_data.h"

#include <format>
#include <cstdint>

#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>

namespace vme::api
{

    static vk_data::attachment _parse_attachment(
        const nlohmann::json& attachment_item)
    {
        vk_data::attachment result;

        std::string type_str =
            attachment_item.at("type").template get<std::string>();
        std::optional<vk_data::attachment_type> type =
            vk_data::attachment_type_from_string(type_str);
        if (!type.has_value())
        {
            throw message_stream_invalid_response_error(
                "Invalid API response: Attachment item has unknown type");
        }

        result.type = type.value();
        const nlohmann::json& attachment = attachment_item.at(type_str);

        switch (type.value())
        {
            using enum vk_data::attachment_type;
        case photo:
            result.photo_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.photo_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.photo_value.date =
                attachment.at("date").template get<std::int64_t>();
            result.photo_value.url = attachment.at("orig_photo")
                                         .at("url")
                                         .template get<std::string>();
            break;

        case video:
        {

            result.video_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.video_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.video_value.date =
                attachment.at("date").template get<std::int64_t>();
            result.video_value.title =
                attachment.at("title").template get<std::string>();
            if (attachment.contains("description"))
            {
                result.video_value.description =
                    attachment.at("description").template get<std::string>();
            }
            else
            {
                result.video_value.description = std::nullopt;
            }
            if (attachment.contains("image"))
            {
                const nlohmann::json& images_array = attachment.at("image");
                const nlohmann::json& url_object =
                    images_array.at(images_array.size()).at("url");
                result.video_value.image_url =
                    url_object.template get<std::string>();
            }
            else
            {
                result.video_value.image_url = std::nullopt;
            }
            break;
        }

        case audio:
            result.audio_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.audio_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.audio_value.artist =
                attachment.at("artist").template get<std::string>();
            result.audio_value.title =
                attachment.at("title").template get<std::string>();
            result.audio_value.duration =
                attachment.at("duration").template get<std::int64_t>();
            break;

        case document:
            result.document_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.document_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.document_value.date =
                attachment.at("date").template get<std::int64_t>();
            result.document_value.title =
                attachment.at("title").template get<std::string>();
            result.document_value.ext =
                attachment.at("ext").template get<std::string>();
            result.document_value.url =
                attachment.at("url").template get<std::string>();
            break;

        case link:
            result.link_value.url =
                attachment.at("url").template get<std::string>();
            result.link_value.title =
                attachment.at("title").template get<std::string>();
            result.link_value.caption =
                attachment.at("caption").template get<std::string>();
            if (attachment.contains("description"))
            {
                result.link_value.description =
                    attachment.at("description").template get<std::string>();
            }
            else
            {
                result.link_value.description = std::nullopt;
            }
            break;

        case product:
            result.product_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.product_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.product_value.title =
                attachment.at("title").template get<std::string>();
            result.product_value.description =
                attachment.at("description").template get<std::string>();
            result.product_value.price =
                attachment.at("price").at("amount").template get<std::string>();
            result.product_value.currency = attachment.at("price")
                                                .at("currency")
                                                .template get<std::string>();
            result.product_value.thumb_url =
                attachment.at("thumb_photo").template get<std::string>();
            break;

        case product_album:
            result.product_album_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.product_album_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.product_album_value.title =
                attachment.at("title").template get<std::string>();
            result.product_album_value.is_main =
                attachment.at("is_main").template get<bool>();
            result.product_album_value.is_hidden =
                attachment.at("is_hidden").template get<bool>();
            break;

        case post:
            result.post_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.post_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.post_value.from_id =
                attachment.at("from_id").template get<std::int64_t>();
            result.post_value.date =
                attachment.at("date").template get<std::int64_t>();
            result.post_value.text =
                attachment.at("text").template get<std::string>();
            break;

        case comment:
            result.comment_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.comment_value.from_id =
                attachment.at("from_id").template get<std::int64_t>();
            result.comment_value.date =
                attachment.at("date").template get<std::int64_t>();
            result.comment_value.text =
                attachment.at("text").template get<std::string>();
            break;

        case sticker:
            result.sticker_value.id =
                attachment.at("sticker_id").template get<std::int64_t>();
            break;

        case gift:
            result.gift_value.id =
                attachment.at("id").template get<std::int64_t>();
            break;

        case call:
            result.call_value.initiator_id =
                attachment.at("initiator_id").template get<std::int64_t>();
            result.call_value.receiver_id =
                attachment.at("receiver_id").template get<std::int64_t>();
            result.call_value.state =
                attachment.at("state").template get<std::string>();
            result.call_value.time =
                attachment.at("time").template get<std::int64_t>();
            result.call_value.duration =
                attachment.at("duration").template get<std::int64_t>();
            result.call_value.video =
                attachment.at("video").template get<bool>();
            break;
        }

        return result;
    }

    static vk_data::message _parse_message(const nlohmann::json& message_item)
    {
        vk_data::message result;

        result.from_id =
            message_item.at("from_id").template get<std::int64_t>();

        result.conversation_message_id =
            message_item.at("conversation_message_id")
                .template get<std::int64_t>();

        result.date = message_item.at("date").template get<std::int64_t>();

        result.important = message_item.at("important").template get<bool>();

        result.text = message_item.at("text").template get<std::string>();

        if (message_item.contains("reply_conversation_message_id"))
        {
            result.reply_conversation_message_id =
                message_item.at("reply_conversation_message_id")
                    .template get<std::int64_t>();
        }
        else
        {
            result.reply_conversation_message_id = std::nullopt;
        }

        if (message_item.contains("fwd_messages"))
        {
            result.fwd_messages.reserve(message_item.at("fwd_messages").size());

            for (const nlohmann::json& fwd_message_item :
                message_item.at("fwd_messages"))
            {
                vk_data::message fwd_message = _parse_message(fwd_message_item);
                result.fwd_messages.push_back(
                    std::make_shared<vk_data::message>(fwd_message));
            }
        }

        if (message_item.contains("attachments"))
        {
            result.attachments.reserve(message_item.at("attachments").size());

            for (const nlohmann::json& attachment_item :
                message_item.at("attachments"))
            {
                result.attachments.push_back(
                    _parse_attachment(attachment_item));
            }
        }

        return result;
    }

    static std::vector<vk_data::message> _parse_messages(
        const std::string& response)
    {
        try
        {
            nlohmann::json response_json = nlohmann::json::parse(response);

            if (response_json.contains("error"))
            {
                nlohmann::json& error_object = response_json.at("error");

                nlohmann::json& error_code_obj = error_object.at("error_code");
                nlohmann::json& error_msg_obj = error_object.at("error_msg");

                throw message_stream_api_error(
                    std::format("API returned an error: {} {}",
                        error_code_obj.template get<int>(),
                        error_msg_obj.template get<std::string>()));
            }

            nlohmann::json& response_body = response_json.at("response");

            nlohmann::json& items = response_body.at("items");

            std::vector<vk_data::message> result;
            result.reserve(items.size());

            for (const nlohmann::json& message_item : items)
            {
                result.push_back(_parse_message(message_item));
            }

            return result;
        }
        catch (const nlohmann::json::exception& e)
        {
            throw message_stream_invalid_response_error(
                std::format("Invalid API response: {}", e.what()));
        }
    }

    message_stream::message_stream(api::session&& s, std::int64_t peer_id,
        std::string access_token) noexcept :
        m_session(std::move(s)),
        m_peer_id(peer_id),
        m_access_token(access_token),
        m_current_offset(0)
    {
    }

    std::optional<vk_data::message> message_stream::next()
    {
        if (m_message_buffer.empty())
        {
            // clang-format off
            std::string response = 
                m_session.call("method/messages.getHistory", {
                    { "offset",       m_current_offset },
                    { "peer_id",      m_peer_id },
                    { "access_token", m_access_token },
                    { "count",        200 },
                    { "rev",          1 },
                    { "v",            "5.199" }
            });
            // clang-format on

            std::vector<vk_data::message> messages = _parse_messages(response);
            m_current_offset += messages.size();

            for (const auto& message : messages)
            {
                m_message_buffer.push(std::move(message));
            }
        }

        if (m_message_buffer.empty())
        {
            return std::nullopt;
        }

        vk_data::message message = std::move(m_message_buffer.front());
        m_message_buffer.pop();
        return std::make_optional(message);
    }

}
