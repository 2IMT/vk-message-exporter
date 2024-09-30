#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <memory>

namespace vme::api::vk_data
{

    struct user
    {
        std::int64_t id;
        std::string first_name;
        std::string last_name;
    };

    struct photo
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::int64_t date;
        std::string url;
    };

    struct video
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::int64_t date;
        std::string title;
        std::optional<std::string> description;
        std::optional<std::string> image_url;
    };

    struct audio
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::string artist;
        std::string title;
        std::int64_t duration;
    };

    struct document
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::int64_t date;
        std::string title;
        std::string ext;
        std::string url;
    };

    struct link
    {
        std::int64_t id;
        std::string url;
        std::string title;
        std::string caption;
        std::optional<std::string> description;
    };

    struct product
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::string title;
        std::string description;
        std::string price;
        std::string currency;
        std::string category_name;
        std::string category_section;
        std::string thumb_url;
    };

    struct product_album
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::string title;
        bool is_main;
        bool is_hidden;
    };

    struct post
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::int64_t from_id;
        std::int64_t date;
        std::string text;
    };

    struct comment
    {
        std::int64_t id;
        std::int64_t from_id;
        std::int64_t date;
        std::string text;
    };

    struct sticker
    {
        std::int64_t id;
    };

    struct gift
    {
        std::int64_t id;
    };

    struct call
    {
        std::int64_t initiator_id;
        std::int64_t receiver_id;
        std::string state;
        std::int64_t time;
        std::int64_t duration;
        bool video;
    };

    enum class attachment_type
    {
        photo,
        video,
        audio,
        document,
        link,
        product,
        product_album,
        post,
        comment,
        sticker,
        gift,
        call
    };

    std::string attachment_type_to_string(attachment_type type) noexcept;

    std::optional<attachment_type> attachment_type_from_string(
        const std::string& type) noexcept;

    struct attachment
    {
        attachment_type type;
        photo photo_value;
        video video_value;
        audio audio_value;
        document document_value;
        link link_value;
        product product_value;
        product_album product_album_value;
        post post_value;
        comment comment_value;
        sticker sticker_value;
        gift gift_value;
        call call_value;
    };

    struct message
    {
        std::int64_t from_id;
        std::int64_t conversation_message_id;
        std::int64_t date;
        bool important;
        std::string text;
        std::optional<std::int64_t> reply_conversation_message_id;
        std::vector<std::shared_ptr<message>> fwd_messages;
        std::vector<attachment> attachments;
        std::string original_json;
    };

}
