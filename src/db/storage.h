#pragma once

#include <string>
#include <unordered_map>

#include "error.h"
#include "db.h"
#include "api/vk_data.h"

namespace vme::db
{

    class storage_io_error : public error
    {
    public:
        storage_io_error(const std::string& message) noexcept :
            error(message)
        {
        }
    };

    class storage
    {
    public:
        storage(const std::string& root, db& database);

        void put(const api::vk_data::message& message);

        void download_media(bool show_progress) const;

    private:
        struct document_url
        {
            document_url() { }

            document_url(const std::string& url, const std::string& ext) :
                url(url),
                ext(ext)
            {
            }

            std::string url;
            std::string ext;
        };

        void pull_links(const api::vk_data::message& message);

        std::string m_root;
        db& m_db;
        std::unordered_map<std::int64_t, std::string> m_photos;
        std::unordered_map<std::int64_t, std::string> m_video_images;
        std::unordered_map<std::int64_t, document_url> m_documents;
        std::unordered_map<std::int64_t, std::string> m_product_thumbs;
        std::unordered_map<std::int64_t, std::string> m_stickers;
        std::unordered_map<std::int64_t, std::string> m_gifts;
        std::unordered_map<std::int64_t, std::string> m_audio_messages;
        std::unordered_map<std::int64_t, std::string> m_graffitis;
    };

}
