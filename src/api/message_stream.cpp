#include "message_stream.h"

#include <format>
#include <cstdint>
#include <cstddef>

#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>

namespace vme::api
{

    static void _parse_audio_attachment(
        const nlohmann::json& item, vk_data::audio& out)
    {
        out.id = item.at("id").template get<std::int64_t>();
        out.owner_id = item.at("owner_id").template get<std::int64_t>();
        out.artist = item.at("artist").template get<std::string>();
        out.title = item.at("title").template get<std::string>();
        out.duration = item.at("duration").template get<std::int64_t>();
    }

    static void _parse_poll_answer(
        const nlohmann::json& item, vk_data::poll_answer& out)
    {
        out.id = item.at("id").template get<std::int64_t>();
        out.rate = item.at("rate").template get<float>();
        out.text = item.at("text").template get<std::string>();
        out.votes = item.at("votes").template get<std::int64_t>();
    }

    static vk_data::attachment _parse_attachment(
        const nlohmann::json& attachment_item, std::int64_t& link_id_counter,
        std::int64_t& call_id_counter)
    {
        vk_data::attachment result;

        std::string type_str =
            attachment_item.at("type").template get<std::string>();
        std::optional<vk_data::attachment_type> type =
            vk_data::attachment_type_from_string(type_str);
        if (!type.has_value())
        {
            throw message_stream_response_error(std::format(
                "Invalid API response: Attachment item has unknown type \"{}\"",
                type_str));
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
                    images_array.at(images_array.size() - 1).at("url");
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
            _parse_audio_attachment(attachment, result.audio_value);
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
            result.link_value.id = link_id_counter++;
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
            if (attachment.at("price").at("currency").is_object())
            {
                result.product_value.currency =
                    attachment.at("price")
                        .at("currency")
                        .at("name")
                        .template get<std::string>();
            }
            else
            {
                result.product_value.currency =
                    attachment.at("price")
                        .at("currency")
                        .template get<std::string>();
            }
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
            result.sticker_value.url = std::format(
                "https://vk.com/sticker/{}/512.png", result.sticker_value.id);
            break;

        case gift:
            result.gift_value.id =
                attachment.at("id").template get<std::int64_t>();
            if (attachment.contains("thumb_512"))
            {
                result.gift_value.url =
                    attachment.at("thumb_512").template get<std::string>();
            }
            else if (attachment.contains("thumb_256"))
            {
                result.gift_value.url =
                    attachment.at("thumb_256").template get<std::string>();
            }
            else if (attachment.contains("thumb_96"))
            {
                result.gift_value.url =
                    attachment.at("thumb_96").template get<std::string>();
            }
            else
            {
                result.gift_value.url =
                    attachment.at("thumb_48").template get<std::string>();
            }
            break;

        case call:
            result.call_value.id = call_id_counter++;
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

        case audio_message:
            result.audio_message_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.audio_message_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.audio_message_value.duration =
                attachment.at("duration").template get<std::int64_t>();
            result.audio_message_value.waveform.reserve(
                attachment.at("waveform").size());
            for (const auto& sample_item : attachment.at("waveform"))
            {
                result.audio_message_value.waveform.push_back(
                    sample_item.template get<std::uint16_t>());
            }
            result.audio_message_value.link_mp3 =
                attachment.at("link_mp3").template get<std::string>();
            break;

        case audio_playlist:
            result.audio_playlist_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.audio_playlist_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.audio_playlist_value.create_time =
                attachment.at("create_time").template get<std::int64_t>();
            result.audio_playlist_value.update_time =
                attachment.at("update_time").template get<std::int64_t>();
            if (attachment.contains("year"))
            {
                result.audio_playlist_value.year =
                    attachment.at("year").template get<std::int64_t>();
            }
            else
            {
                result.audio_playlist_value.year = std::nullopt;
            }
            result.audio_playlist_value.title =
                attachment.at("title").template get<std::string>();
            result.audio_playlist_value.description =
                attachment.at("description").template get<std::string>();
            result.audio_playlist_value.audios.reserve(
                attachment.at("audios").size());
            for (const auto& audio_item : attachment.at("audios"))
            {
                vk_data::audio parsed_audio;
                _parse_audio_attachment(audio_item, parsed_audio);
                result.audio_playlist_value.audios.push_back(parsed_audio);
            }
            break;

        case graffiti:
            result.graffiti_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.graffiti_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.graffiti_value.url =
                attachment.at("url").template get<std::string>();
            break;

        case money_request:
            result.money_request_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.money_request_value.from_id =
                attachment.at("from_id").template get<std::int64_t>();
            result.money_request_value.to_id =
                attachment.at("to_id").template get<std::int64_t>();
            result.money_request_value.amount =
                attachment.at("amount")
                    .at("amount")
                    .template get<std::string>();
            if (attachment.at("amount").at("currency").is_object())
            {
                result.money_request_value.currency =
                    attachment.at("amount")
                        .at("currency")
                        .at("name")
                        .template get<std::string>();
            }
            else
            {
                result.money_request_value.currency =
                    attachment.at("amount")
                        .at("currency")
                        .template get<std::string>();
            }
            break;

        case story:
            result.story_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.story_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.story_value.date =
                attachment.at("date").template get<std::int64_t>();
            result.story_value.expires_at =
                attachment.at("expires_at").template get<std::int64_t>();
            break;

        case poll:
            result.poll_value.id =
                attachment.at("id").template get<std::int64_t>();
            result.poll_value.owner_id =
                attachment.at("owner_id").template get<std::int64_t>();
            result.poll_value.question =
                attachment.at("question").template get<std::string>();
            result.poll_value.votes =
                attachment.at("votes").template get<std::int64_t>();
            for (const auto& item : attachment.at("answers"))
            {
                vk_data::poll_answer poll_answer;
                _parse_poll_answer(item, poll_answer);
                result.poll_value.answers.push_back(poll_answer);
            }
            break;
        }

        return result;
    }

    static vk_data::message _parse_message(const nlohmann::json& message_item,
        std::int64_t& link_id_counter, std::int64_t& call_id_counter)
    {
        vk_data::message result;

        result.original_json = message_item.dump();

        if (message_item.contains("id"))
        {
            result.id = message_item.at("id").template get<std::int64_t>();
        }
        else
        {
            result.id = std::nullopt;
        }

        result.from_id =
            message_item.at("from_id").template get<std::int64_t>();

        result.conversation_message_id =
            message_item.at("conversation_message_id")
                .template get<std::int64_t>();

        result.date = message_item.at("date").template get<std::int64_t>();

        if (message_item.contains("important"))
        {
            result.important =
                message_item.at("important").template get<bool>();
        }
        else
        {
            result.important = false;
        }

        result.text = message_item.at("text").template get<std::string>();

        if (message_item.contains("reply_conversation_message_id"))
        {
            result.reply_conversation_message_id =
                message_item.at("reply_conversation_message_id")
                    .template get<std::int64_t>();
        }
        else if (message_item.contains("reply_message"))
        {
            result.reply_conversation_message_id =
                message_item.at("reply_message")
                    .at("conversation_message_id")
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
                vk_data::message fwd_message = _parse_message(
                    fwd_message_item, link_id_counter, call_id_counter);
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
                result.attachments.push_back(_parse_attachment(
                    attachment_item, link_id_counter, call_id_counter));
            }
        }

        return result;
    }

    static std::vector<vk_data::message> _parse_messages(
        const std::string& response, std::size_t& count,
        std::int64_t& link_id_counter, std::int64_t& call_id_counter)
    {
        try
        {
            nlohmann::json response_json = nlohmann::json::parse(response);

            if (response_json.contains("error"))
            {
                nlohmann::json& error_object = response_json.at("error");

                nlohmann::json& error_code_obj = error_object.at("error_code");
                nlohmann::json& error_msg_obj = error_object.at("error_msg");

                throw message_stream_response_error(
                    std::format("API returned an error: {} {}",
                        error_code_obj.template get<int>(),
                        error_msg_obj.template get<std::string>()));
            }

            nlohmann::json response_body = response_json.at("response");

            nlohmann::json& count_obj = response_body.at("count");
            count = count_obj.template get<std::size_t>();

            nlohmann::json& items = response_body.at("items");

            std::vector<vk_data::message> result;
            result.reserve(items.size());

            for (const nlohmann::json& message_item : items)
            {
                result.push_back(_parse_message(
                    message_item, link_id_counter, call_id_counter));
            }

            return result;
        }
        catch (const nlohmann::json::exception& e)
        {
            throw message_stream_response_error(
                std::format("Invalid API response: {}", e.what()));
        }
    }

    message_stream::message_stream(api::session& s, std::int64_t peer_id,
        const std::string& access_token) noexcept :
        m_session(s),
        m_peer_id(peer_id),
        m_access_token(access_token),
        m_current_offset(0),
        m_message_count(0),
        m_link_id_counter(0),
        m_call_id_counter(0)
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

            std::vector<vk_data::message> messages = _parse_messages(response,
                m_message_count, m_link_id_counter, m_call_id_counter);
            m_current_offset += 200;

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

    std::size_t message_stream::message_count() const noexcept
    {
        return m_message_count;
    }

}
