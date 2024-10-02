#include "vk_data.h"

namespace vme::api::vk_data
{

    std::string attachment_type_to_string(attachment_type type) noexcept
    {
        switch (type)
        {
            using enum attachment_type;
        case photo:
            return "photo";
        case video:
            return "video";
        case audio:
            return "audio";
        case document:
            return "doc";
        case link:
            return "link";
        case product:
            return "market";
        case product_album:
            return "market_album";
        case post:
            return "wall";
        case comment:
            return "wall_reply";
        case sticker:
            return "sticker";
        case gift:
            return "gift";
        case call:
            return "call";
        case audio_message:
            return "audio_message";
        case audio_playlist:
            return "audio_playlist";
        case graffiti:
            return "graffiti";
        default:
            return "unknown";
        }
    }

    std::optional<attachment_type> attachment_type_from_string(
        const std::string& type) noexcept
    {
        using enum attachment_type;

        if (type == "photo")
        {
            return photo;
        }
        if (type == "video")
        {
            return video;
        }
        if (type == "audio")
        {
            return audio;
        }
        if (type == "doc")
        {
            return document;
        }
        if (type == "link")
        {
            return link;
        }
        if (type == "market")
        {
            return product;
        }
        if (type == "market_album")
        {
            return product_album;
        }
        if (type == "wall")
        {
            return post;
        }
        if (type == "wall_reply")
        {
            return comment;
        }
        if (type == "sticker")
        {
            return sticker;
        }
        if (type == "gift")
        {
            return gift;
        }
        if (type == "call")
        {
            return call;
        }
        if (type == "audio_message")
        {
            return audio_message;
        }
        if (type == "audio_playlist")
        {
            return audio_playlist;
        }
        if (type == "graffiti")
        {
            return graffiti;
        }

        return std::nullopt;
    }

}
