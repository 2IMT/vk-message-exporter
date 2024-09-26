#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <memory>

namespace vme::vk_data
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
    };

    struct video
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::int64_t date;
        std::string title;
        std::string description;
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
    };

    struct link
    {
        std::int64_t id;
        std::string url;
        std::string title;
        std::string caption;
        std::string description;
        bool has_photo;
    };

    struct product
    {
        std::int64_t id;
        std::int64_t owner_id;
        std::int64_t date;
        std::string title;
        std::string description;
        std::string price;
        std::string currency;
        std::string category_name;
        std::string category_section;
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

    enum class attachment_type
    {
        photo,
        video,
        audio,
        document,
        link,
        product,
        post,
        comment,
        sticker,
        gift,
    };

    union attachment_value
    {
        photo photo_value;
        video video_value;
        audio audio_value;
        document document_value;
        link link_value;
        product product_value;
        post post_value;
        comment comment_value;
        sticker sticker_value;
        gift gift_value;
    };

    struct attachment
    {
        attachment_type type;
        attachment_value value;
    };

    struct message
    {
        std::int64_t from_id;
        std::int64_t conversation_message_id;
        std::int64_t date;
        bool important;
        std::string text;
        std::optional<std::int64_t> reply_conversation_message_id;
        std::vector<std::unique_ptr<message>> fwd_messages;
        std::vector<attachment> attachments;
    };

}
