#include "vk_data.h"

namespace vme::vk_data
{

    std::string attachment_type_to_string(attachment_type type) noexcept
    {
        switch (type)
        {
            using enum attachment_type;
        case photo:
            return "photo";
            break;
        case video:
            return "video";
            break;
        case audio:
            return "audio";
            break;
        case document:
            return "doc";
            break;
        case link:
            return "link";
            break;
        case product:
            return "market";
            break;
        case post:
            return "wall";
            break;
        case comment:
            return "wall_reply";
            break;
        case sticker:
            return "sticker";
            break;
        case gift:
            return "gift";
            break;
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

        return std::nullopt;
    }

}
