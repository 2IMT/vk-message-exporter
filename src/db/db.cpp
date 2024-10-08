#include "db.h"

#include <format>
#include <cstdint>
#include <cstddef>
#include <filesystem>

#include <sqlite3.h>

#include "api/vk_data.h"
#include "sql.h"

namespace vme::db
{

    class sql_null_type
    {
    };

    static const sql_null_type sql_null;

    class sql_int
    {
    public:
        sql_int(std::int64_t value) :
            value(value),
            is_null(false)
        {
        }

        sql_int(sql_null_type) :
            value(0),
            is_null(true)
        {
        }

        bool is_null;
        std::int64_t value;
    };

    class sql_real
    {
    public:
        sql_real(double value) :
            value(value),
            is_null(false)
        {
        }

        sql_real(sql_null_type) :
            value(0),
            is_null(true)
        {
        }

        bool is_null;
        double value;
    };

    class sql_text
    {
    public:
        sql_text(const std::string& value) :
            value(value),
            is_null(false)
        {
        }

        sql_text(sql_null_type) :
            is_null(true)
        {
        }

        bool is_null;
        std::string value;
    };

    class sql_blob
    {
    public:
        sql_blob(const unsigned char* data, std::size_t len) :
            value(data, data + len),
            is_null(false)
        {
        }

        sql_blob(sql_null_type) :
            is_null(true)
        {
        }

        bool is_null;
        std::vector<unsigned char> value;
    };

    template<class Type>
    static int _bind_stmt(sqlite3_stmt* stmt, int pos, const Type& value);

    template<>
    int _bind_stmt(sqlite3_stmt* stmt, int pos, const sql_int& value)
    {
        if (value.is_null)
        {
            return sqlite3_bind_null(stmt, pos);
        }

        return sqlite3_bind_int64(stmt, pos, value.value);
    }

    template<>
    int _bind_stmt(sqlite3_stmt* stmt, int pos, const sql_real& value)
    {
        if (value.is_null)
        {
            return sqlite3_bind_null(stmt, pos);
        }

        return sqlite3_bind_double(stmt, pos, value.value);
    }

    template<>
    int _bind_stmt(sqlite3_stmt* stmt, int pos, const sql_text& value)
    {
        if (value.is_null)
        {
            return sqlite3_bind_null(stmt, pos);
        }

        return sqlite3_bind_text(stmt, pos, value.value.c_str(),
            value.value.length(), SQLITE_TRANSIENT);
    }

    template<>
    int _bind_stmt(sqlite3_stmt* stmt, int pos, const sql_blob& value)
    {
        if (value.is_null)
        {
            return sqlite3_bind_null(stmt, pos);
        }

        return sqlite3_bind_blob(stmt, pos, value.value.data(),
            value.value.size(), SQLITE_TRANSIENT);
    }

    class sql_result
    {
    public:
        sql_result(sqlite3_stmt* stmt, bool is_row) :
            m_stmt(stmt),
            m_is_row(is_row)
        {
        }

        sql_result(const sql_result& other) = delete;

        sql_result(sql_result&& other) :
            m_stmt(other.m_stmt)
        {
            other.m_stmt = nullptr;
        }

        sql_result& operator=(const sql_result& other) = delete;

        sql_result& operator=(sql_result&& other)
        {
            m_stmt = other.m_stmt;
            other.m_stmt = nullptr;

            return *this;
        }

        ~sql_result()
        {
            if (m_stmt != nullptr)
            {
                sqlite3_finalize(m_stmt);
            }
        }

        bool get_bool(std::size_t col)
        {
            if (!m_is_row)
            {
                throw db_operation_error(
                    "Database operation error: result is not a row");
            }

            return sqlite3_column_int(m_stmt, col);
        }

        std::int64_t get_int64(std::size_t col)
        {
            if (!m_is_row)
            {
                throw db_operation_error(
                    "Database operation error: result is not a row");
            }

            return sqlite3_column_int64(m_stmt, col);
        }

        double get_double(std::size_t col)
        {
            if (!m_is_row)
            {
                throw db_operation_error(
                    "Database operation error: result is not a row");
            }

            return sqlite3_column_double(m_stmt, col);
        }

        std::string get_text(std::size_t col)
        {
            if (!m_is_row)
            {
                throw db_operation_error(
                    "Database operation error: result is not a row");
            }

            const unsigned char* text = sqlite3_column_text(m_stmt, col);
            return text ? reinterpret_cast<const char*>(text) : "";
        }

    private:
        bool m_is_row;
        sqlite3_stmt* m_stmt;
    };

    template<class... Args>
    sql_result _execute_stmt(
        sqlite3* db, const std::string& stmt, Args&&... args)
    {
        sqlite3_stmt* statement = nullptr;
        int status = sqlite3_prepare_v2(
            db, stmt.c_str(), stmt.length(), &statement, nullptr);

        if (status != SQLITE_OK)
        {
            throw db_operation_error(std::format(
                "Database operation error: {}", sqlite3_errmsg(db)));
        }

        int i = 1;
        (static_cast<void>(
             [&]
             {
                 int status = _bind_stmt(statement, i, args);
                 if (status != SQLITE_OK)
                 {
                     sqlite3_finalize(statement);
                     throw db_operation_error(std::format(
                         "Database operation error: {}", sqlite3_errmsg(db)));
                 }

                 i++;
             }()),
            ...);

        status = sqlite3_step(statement);
        if (status != SQLITE_DONE && status != SQLITE_ROW)
        {
            sqlite3_finalize(statement);
            throw db_operation_error(std::format(
                "Database operation error: {}", sqlite3_errmsg(db)));
        }

        return sql_result(statement, status == SQLITE_ROW);
    }

    static void _put_audio(sqlite3* db, const api::vk_data::audio& audio)
    {
        sql_result result =
            _execute_stmt(db, sql::exists_audio, sql_int(audio.id));
        if (result.get_bool(0))
        {
            return;
        }

        _execute_stmt(db, sql::insert_audio, sql_int(audio.id),
            sql_int(audio.owner_id), sql_text(audio.artist),
            sql_text(audio.title), sql_int(audio.duration));
    }

    db::db(const std::string& db_file_path)
    {
        try
        {
            if (std::filesystem::exists(db_file_path))
            {
                if (!std::filesystem::is_regular_file(db_file_path))
                {
                    throw db_init_error(std::format(
                        "Database file \"{}\" already exists and is "
                        "not a regular file",
                        db_file_path));
                }

                std::filesystem::remove(db_file_path);
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            throw db_init_error(
                std::format("Database filesystem error: {}", e.what()));
        }

        int status = sqlite3_open_v2(db_file_path.c_str(), &m_sqlite3,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        if (status)
        {
            throw db_init_error(
                std::format("Failed to open database \"{}\": {}", db_file_path,
                    sqlite3_errmsg(m_sqlite3)));
        }

        char* message = nullptr;
        status = sqlite3_exec(
            m_sqlite3, sql::init.c_str(), nullptr, nullptr, &message);
        if (status)
        {
            throw db_init_error(
                std::format("Failed to initialize database \"{}\": {}",
                    db_file_path, message));
            sqlite3_free(message);
        }
    }

    db::db(db&& other) noexcept :
        m_sqlite3(other.m_sqlite3)
    {
        other.m_sqlite3 = nullptr;
    }

    db& db::operator=(db&& other) noexcept
    {
        sqlite3_close_v2(m_sqlite3);
        m_sqlite3 = other.m_sqlite3;
        other.m_sqlite3 = nullptr;

        return *this;
    }

    db::~db() noexcept
    {
        if (m_sqlite3 != nullptr)
        {
            sqlite3_close_v2(m_sqlite3);
        }
    }

    void db::put(const api::vk_data::message& message)
    {
        sql_int id(sql_null);
        if (message.id.has_value())
        {
            id = sql_int(message.id.value());
        }

        sql_result result = _execute_stmt(m_sqlite3, sql::exists_message, id,
            sql_int(message.from_id), sql_int(message.conversation_message_id));
        if (result.get_bool(0))
        {
            return;
        }

        sql_int rcmi(sql_null);
        if (message.reply_conversation_message_id.has_value())
        {
            rcmi = sql_int(message.reply_conversation_message_id.value());
        }

        _execute_stmt(m_sqlite3, sql::insert_message, id,
            sql_int(message.from_id), sql_int(message.conversation_message_id),
            sql_int(message.date), sql_int(message.important),
            sql_text(message.text), rcmi, sql_text(message.original_json));

        std::int64_t attachment_index = 0;
        for (const auto& attachment : message.attachments)
        {
            std::int64_t attachment_id = 0;

            switch (attachment.type)
            {
                using enum api::vk_data::attachment_type;
            case photo:
            {
                attachment_id = attachment.photo_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_photo, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_photo,
                    sql_int(attachment.photo_value.id),
                    sql_int(attachment.photo_value.owner_id),
                    sql_int(attachment.photo_value.date));
                break;
            }

            case video:
            {
                attachment_id = attachment.video_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_video, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                sql_text description(sql_null);
                if (attachment.video_value.description.has_value())
                {
                    description = attachment.video_value.description.value();
                }

                _execute_stmt(m_sqlite3, sql::insert_video,
                    sql_int(attachment.video_value.id),
                    sql_int(attachment.video_value.owner_id),
                    sql_int(attachment.video_value.date),
                    sql_text(attachment.video_value.title), description);
                break;
            }

            case audio:
            {
                attachment_id = attachment.audio_value.id;
                _put_audio(m_sqlite3, attachment.audio_value);

                break;
            }

            case document:
            {
                attachment_id = attachment.document_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_document, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_document,
                    sql_int(attachment.document_value.id),
                    sql_int(attachment.document_value.owner_id),
                    sql_int(attachment.document_value.date),
                    sql_text(attachment.document_value.title),
                    sql_text(attachment.document_value.ext));
                break;
            }

            case link:
            {
                attachment_id = attachment.link_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_link, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                sql_text description(sql_null);
                if (attachment.link_value.description.has_value())
                {
                    description = attachment.link_value.description.value();
                }

                _execute_stmt(m_sqlite3, sql::insert_link,
                    sql_int(attachment.link_value.id),
                    sql_text(attachment.link_value.title),
                    sql_text(attachment.link_value.url),
                    sql_text(attachment.link_value.caption), description);
                break;
            }

            case product:
            {
                attachment_id = attachment.product_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_product, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_product,
                    sql_int(attachment.product_value.id),
                    sql_int(attachment.product_value.owner_id),
                    sql_text(attachment.product_value.title),
                    sql_text(attachment.product_value.description),
                    sql_text(attachment.product_value.price),
                    sql_text(attachment.product_value.currency),
                    sql_text(attachment.product_value.category_name),
                    sql_text(attachment.product_value.category_section));
                break;
            }

            case product_album:
            {
                attachment_id = attachment.product_album_value.id;

                sql_result result = _execute_stmt(m_sqlite3,
                    sql::exists_product_album, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_product_album,
                    sql_int(attachment.product_album_value.id),
                    sql_int(attachment.product_album_value.owner_id),
                    sql_text(attachment.product_album_value.title),
                    sql_int(attachment.product_album_value.is_main),
                    sql_int(attachment.product_album_value.is_hidden));
                break;
            }

            case post:
            {
                attachment_id = attachment.post_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_post, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_post,
                    sql_int(attachment.post_value.id),
                    sql_int(attachment.post_value.owner_id),
                    sql_int(attachment.post_value.from_id),
                    sql_int(attachment.post_value.date),
                    sql_text(attachment.post_value.text));
                break;
            }

            case comment:
            {
                attachment_id = attachment.comment_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_comment, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_comment,
                    sql_int(attachment.comment_value.id),
                    sql_int(attachment.comment_value.from_id),
                    sql_int(attachment.comment_value.date),
                    sql_text(attachment.comment_value.text));
                break;
            }

            case sticker:
            {
                attachment_id = attachment.sticker_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_sticker, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_sticker,
                    sql_int(attachment.sticker_value.id));
                break;
            }

            case gift:
            {
                attachment_id = attachment.gift_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_gift, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_gift,
                    sql_int(attachment.gift_value.id));
                break;
            }

            case call:
            {
                attachment_id = attachment.call_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_call, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_call,
                    sql_int(attachment.call_value.id),
                    sql_int(attachment.call_value.initiator_id),
                    sql_int(attachment.call_value.receiver_id),
                    sql_text(attachment.call_value.state),
                    sql_int(attachment.call_value.time),
                    sql_int(attachment.call_value.duration),
                    sql_int(attachment.call_value.video));
                break;
            }

            case audio_message:
            {
                attachment_id = attachment.audio_message_value.id;

                sql_result result = _execute_stmt(m_sqlite3,
                    sql::exists_audio_message, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                const std::uint16_t* data =
                    attachment.audio_message_value.waveform.data();
                std::size_t len =
                    attachment.audio_message_value.waveform.size() *
                    sizeof(std::int16_t);
                sql_blob waveform(
                    reinterpret_cast<const unsigned char*>(data), len);

                _execute_stmt(m_sqlite3, sql::insert_audio_message,
                    sql_int(attachment.audio_message_value.id),
                    sql_int(attachment.audio_message_value.owner_id),
                    sql_int(attachment.audio_message_value.duration), waveform);
                break;
            }

            case audio_playlist:
            {
                attachment_id = attachment.audio_playlist_value.id;

                sql_result result = _execute_stmt(m_sqlite3,
                    sql::exists_audio_playlist, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_audio_playlist,
                    sql_int(attachment.audio_playlist_value.id),
                    sql_int(attachment.audio_playlist_value.owner_id),
                    sql_int(attachment.audio_playlist_value.create_time),
                    sql_int(attachment.audio_playlist_value.update_time),
                    sql_int(attachment.audio_playlist_value.year),
                    sql_text(attachment.audio_playlist_value.title),
                    sql_text(attachment.audio_playlist_value.description));

                for (const auto& audio : attachment.audio_playlist_value.audios)
                {
                    _put_audio(m_sqlite3, audio);
                    _execute_stmt(m_sqlite3, sql::insert_audio_playlist_audio,
                        sql_int(audio.id), sql_int(attachment_id));
                }
                break;
            }

            case graffiti:
            {
                attachment_id = attachment.graffiti_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_graffiti, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_graffiti,
                    sql_int(attachment.graffiti_value.id),
                    sql_int(attachment.graffiti_value.owner_id));

                break;
            }

            case money_request:
            {
                attachment_id = attachment.money_request_value.id;

                sql_result result = _execute_stmt(m_sqlite3,
                    sql::exists_money_request, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_money_request,
                    sql_int(attachment.money_request_value.id),
                    sql_int(attachment.money_request_value.from_id),
                    sql_int(attachment.money_request_value.to_id),
                    sql_text(attachment.money_request_value.amount),
                    sql_text(attachment.money_request_value.currency));

                break;
            }

            case story:
            {
                attachment_id = attachment.story_value.id;

                sql_result result = _execute_stmt(
                    m_sqlite3, sql::exists_story, sql_int(attachment_id));
                if (result.get_bool(0))
                {
                    break;
                }

                _execute_stmt(m_sqlite3, sql::insert_story,
                    sql_int(attachment.story_value.id),
                    sql_int(attachment.story_value.owner_id),
                    sql_int(attachment.story_value.date),
                    sql_int(attachment.story_value.expires_at));

                break;
            }
            }

            _execute_stmt(m_sqlite3, sql::insert_message_attachment,
                sql_int(message.from_id),
                sql_int(message.conversation_message_id),
                sql_int(attachment_index), sql_int(attachment_id),
                sql_text(
                    api::vk_data::attachment_type_to_string(attachment.type)));

            attachment_index++;
        }

        std::int64_t fwd_message_index = 0;
        for (const auto& fwd_message : message.fwd_messages)
        {
            _execute_stmt(m_sqlite3, sql::insert_forwarded_message,
                sql_int(message.from_id),
                sql_int(message.conversation_message_id),
                sql_int(fwd_message->from_id),
                sql_int(fwd_message->conversation_message_id),
                sql_int(fwd_message_index));

            put(*fwd_message.get());
            fwd_message_index++;
        }
    }

    void db::put(const api::user_pool& user_pool)
    {
        for (const auto& [id, user] : user_pool)
        {
            sql_result result =
                _execute_stmt(m_sqlite3, sql::exists_user, sql_int(id));
            if (result.get_bool(0))
            {
                continue;
            }

            _execute_stmt(m_sqlite3, sql::insert_user, sql_int(id),
                sql_text(user.first_name), sql_text(user.last_name));
        }
    }

}
