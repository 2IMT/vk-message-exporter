#include "storage.h"

#include <iostream>
#include <fstream>
#include <format>
#include <filesystem>

#include "curl.h"

namespace vme::db
{

    static std::ofstream _open_ofstream(const std::string& path)
    {
        std::ofstream result;
        result.exceptions(std::ios::badbit | std::ios::failbit);
        result.open(path, std::ios::trunc | std::ios::binary);
        return result;
    }

    static void _show_downloading(
        const std::string& what, bool show_progress) noexcept
    {
        if (show_progress)
        {
            std::cout << "Downloading " << what << "..." << std::endl;
        }
    }

    static void _show_count(const std::string& what, std::size_t count,
        std::size_t total, bool show_progress) noexcept
    {
        if (show_progress)
        {
            std::cout << "Downloaded " << count << "/" << total << " " << what
                      << " ("
                      << static_cast<float>(count) / static_cast<float>(total) *
                             100
                      << "%)" << std::endl;
        }
    }

    static void _create_directory(const std::string& root, const std::string& name)
    {
        std::filesystem::path path = std::format("{}/{}", root, name);
        try
        {
            if (std::filesystem::exists(path))
            {
                if (!std::filesystem::is_directory(path))
                {
                    throw storage_io_error(
                        std::format("Filesystem error: \"{}\" exists and is "
                                    "not a directory",
                            path.string()));
                }

                return;
            }

            std::filesystem::create_directory(path);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            throw storage_io_error(std::format(
                "IO error: Failed to ensure directory \"{}\" exists: {}", path.string(),
                e.what()));
        }
    }

    storage::storage(const std::string& root, db& database) :
        m_root(root),
        m_db(database)
    {
    }

    void storage::put(const api::vk_data::message& message)
    {
        pull_links(message);
        m_db.put(message);
    }

    void storage::download_media(bool show_progress) const
    {
        try
        {
            std::size_t count = 0;
            std::size_t total = m_photos.size();
            _show_downloading("photos", show_progress);
            _create_directory(m_root, "photos");
            for (const auto& [id, url] : m_photos)
            {
                std::ofstream ofstream =
                    _open_ofstream(std::format("{}/photos/{}.png", m_root, id));
                curl c;
                c.perform(url, ofstream);
                count++;
                _show_count("photos", count, total, show_progress);
            }

            count = 0;
            total = m_video_images.size();
            _show_downloading("video images", show_progress);
            _create_directory(m_root, "video_images");
            for (const auto& [id, url] : m_video_images)
            {
                std::ofstream ofstream = _open_ofstream(
                    std::format("{}/video_images/{}.png", m_root, id));
                curl c;
                c.perform(url, ofstream);
                count++;
                _show_count("video images", count, total, show_progress);
            }

            count = 0;
            total = m_documents.size();
            _show_downloading("documents", show_progress);
            _create_directory(m_root, "documents");
            for (const auto& [id, url] : m_documents)
            {
                std::ofstream ofstream = _open_ofstream(
                    std::format("{}/documents/{}.{}", m_root, id, url.ext));
                curl c;
                c.perform(url.url, ofstream);
                count++;
                _show_count("documents", count, total, show_progress);
            }

            count = 0;
            total = m_product_thumbs.size();
            _show_downloading("product thumbs", show_progress);
            _create_directory(m_root, "product_thumbs");
            for (const auto& [id, url] : m_product_thumbs)
            {
                std::ofstream ofstream = _open_ofstream(
                    std::format("{}/product_thumbs/{}.png", m_root, id));
                curl c;
                c.perform(url, ofstream);
                count++;
                _show_count("product thumbs", count, total, show_progress);
            }

            count = 0;
            total = m_stickers.size();
            _show_downloading("stickers", show_progress);
            _create_directory(m_root, "stickers");
            for (const auto& [id, url] : m_stickers)
            {
                std::ofstream ofstream = _open_ofstream(
                    std::format("{}/stickers/{}.png", m_root, id));
                curl c;
                c.perform(url, ofstream);
                count++;
                _show_count("stickers", count, total, show_progress);
            }

            count = 0;
            total = m_gifts.size();
            _show_downloading("gifts", show_progress);
            _create_directory(m_root, "gifts");
            for (const auto& [id, url] : m_gifts)
            {
                std::ofstream ofstream =
                    _open_ofstream(std::format("{}/gifts/{}.png", m_root, id));
                curl c;
                c.perform(url, ofstream);
                count++;
                _show_count("gifts", count, total, show_progress);
            }

            count = 0;
            total = m_audio_messages.size();
            _show_downloading("audio messages", show_progress);
            _create_directory(m_root, "audio_messages");
            for (const auto& [id, url] : m_audio_messages)
            {
                std::ofstream ofstream = _open_ofstream(
                    std::format("{}/audio_messages/{}.mp3", m_root, id));
                curl c;
                c.perform(url, ofstream);
                count++;
                _show_count("audio messages", count, total, show_progress);
            }

            count = 0;
            total = m_graffitis.size();
            _show_downloading("graffitis", show_progress);
            _create_directory(m_root, "graffitis");
            for (const auto& [id, url] : m_graffitis)
            {
                std::ofstream ofstream = _open_ofstream(
                    std::format("{}/graffitis/{}.png", m_root, id));
                curl c;
                c.perform(url, ofstream);
                count++;
                _show_count("graffitis", count, total, show_progress);
            }
        }
        catch (const std::ios::failure&)
        {
            throw storage_io_error("IO error");
        }
    }

    void storage::pull_links(const api::vk_data::message& message)
    {
        for (const auto& attachment : message.attachments)
        {
            switch (attachment.type)
            {
                using enum api::vk_data::attachment_type;
            case photo:
                m_photos[attachment.photo_value.id] =
                    attachment.photo_value.url;
                break;
            case video:
                if (attachment.video_value.image_url.has_value())
                {
                    m_video_images[attachment.video_value.id] =
                        attachment.video_value.image_url.value();
                }
                break;
            case document:
                m_documents[attachment.document_value.id] =
                    document_url(attachment.document_value.url,
                        attachment.document_value.ext);
                break;
            case product:
                m_product_thumbs[attachment.product_value.id] =
                    attachment.product_value.thumb_url;
                break;
            case sticker:
                m_stickers[attachment.sticker_value.id] =
                    attachment.sticker_value.url;
                break;
            case gift:
                m_gifts[attachment.gift_value.id] = attachment.gift_value.url;
                break;
            case audio_message:
                m_audio_messages[attachment.audio_message_value.id] =
                    attachment.audio_message_value.link_mp3;
                break;
            case graffiti:
                m_graffitis[attachment.graffiti_value.id] =
                    attachment.graffiti_value.url;
                break;
            default:
                break;
            }
        }

        for (const auto& fwd_message : message.fwd_messages)
        {
            pull_links(*fwd_message.get());
        }
    }

}
