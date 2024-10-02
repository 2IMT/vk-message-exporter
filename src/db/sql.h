#pragma once

#include <string>

namespace vme::db::sql
{

    static inline const std::string init = R""""(
CREATE TABLE IF NOT EXISTS messages (
    from_id INTEGER NOT NULL,
    conversation_message_id INTEGER NOT NULL,
    date INTEGER NOT NULL,
    important INTEGER NOT NULL,
    text TEXT NOT NULL,
    reply_conversation_message_id INTEGER,
    original_json TEXT NOT NULL,
    PRIMARY KEY (from_id, conversation_message_id)
);

CREATE INDEX IF NOT EXISTS idx_messages_from_id_date ON messages(from_id, date);

CREATE TABLE IF NOT EXISTS forwarded_messages (
    message_from_id INTEGER NOT NULL,
    message_conversation_message_id INTEGER NOT NULL,
    forwarded_from_id INTEGER NOT NULL,
    forwarded_conversation_message_id INTEGER NOT NULL,
    sequence_number INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS 
    idx_forwarded_messages_message_from_id_message_conversation_message_id
    ON forwarded_messages(message_from_id, message_conversation_message_id);

CREATE TABLE IF NOT EXISTS users (
    id INTEGER NOT NULL PRIMARY KEY,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS photos (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    date INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS videos (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    date INTEGER NOT NULL,
    title TEXT NOT NULL,
    description TEXT
);

CREATE TABLE IF NOT EXISTS audios (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    artist TEXT NOT NULL,
    title TEXT NOT NULL,
    duration INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS documents (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    date INTEGER NOT NULL,
    title TEXT NOT NULL,
    ext TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS links (
    id INTEGER NOT NULL PRIMARY KEY,
    url TEXT NOT NULL,
    title TEXT NOT NULL,
    caption TEXT NOT NULL,
    description TEXT
);

CREATE TABLE IF NOT EXISTS products (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    title TEXT NOT NULL,
    description TEXT NOT NULL,
    price TEXT NOT NULL,
    currency TEXT NOT NULL,
    category_name TEXT NOT NULL,
    category_section TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS product_albums (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    title TEXT NOT NULL,
    is_main INTEGER NOT NULL,
    is_hidden INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS posts (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    from_id INTEGER NOT NULL,
    date INTEGER NOT NULL,
    text TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS comments (
    id INTEGER NOT NULL PRIMARY KEY,
    from_id INTEGER NOT NULL,
    date INTEGER NOT NULL,
    text TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS stickers (
    id INTEGER NOT NULL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS gifts (
    id INTEGER NOT NULL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS calls (
    id INTEGER NOT NULL PRIMARY KEY,
    initiator_id INTEGER NOT NULL,
    receiver_id INTEGER NOT NULL,
    state TEXT NOT NULL,
    time INTEGER NOT NULL,
    duration INTEGER NOT NULL,
    video INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS audio_messages (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    duration INTEGER NOT NULL,
    waveform BLOB NOT NULL
);

CREATE TABLE IF NOT EXISTS audio_playlists (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    create_time INTEGER NOT NULL,
    update_time INTEGER NOT NULL,
    year INTEGER NOT NULL,
    title TEXT NOT NULL,
    description TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS audio_playlist_audios (
    audio_id INTEGER NOT NULL,
    audio_playlist_id INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_audio_playlist_audios_audio_id_audio_playlist_id
    ON audio_playlist_audios(audio_id, audio_playlist_id);

CREATE TABLE IF NOT EXISTS graffitis (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS money_requests (
    id INTEGER NOT NULL PRIMARY KEY,
    from_id INTEGER NOT NULL,
    to_id INTEGER NOT NULL,
    amount TEXT NOT NULL,
    currency TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS stories (
    id INTEGER NOT NULL PRIMARY KEY,
    owner_id INTEGER NOT NULL,
    date INTEGER NOT NULL,
    expires_at INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS message_attachments (
    message_from_id INTEGER NOT NULL,
    message_conversation_message_id INTEGER NOT NULL,
    sequence_number INTEGER NOT NULL,
    attachment_id INTEGER NOT NULL,
    attachment_type TEXT NOT NULL
);

CREATE INDEX IF NOT EXISTS 
    idx_message_attachments_message_from_id_message_conversation_message_id
    ON message_attachments(message_from_id, message_conversation_message_id);
)"""";

    static inline const std::string exists_message = R""""(
SELECT EXISTS (SELECT * FROM messages WHERE 
               from_id = ?1 AND conversation_message_id = ?2);
)"""";

    static inline const std::string insert_message = R""""(
INSERT INTO messages
(from_id, conversation_message_id, date, important, text,
reply_conversation_message_id, original_json)
VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);
)"""";

    static inline const std::string insert_forwarded_message = R""""(
INSERT INTO forwarded_messages
(message_from_id, message_conversation_message_id, forwarded_from_id,
forwarded_conversation_message_id, sequence_number)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string insert_user = R""""(
INSERT INTO users (id, first_name, last_name)
VALUES (?1, ?2, ?3);
)"""";

    static inline const std::string exists_user = R""""(
SELECT EXISTS (SELECT * FROM users WHERE id = ?1);
)"""";

    static inline const std::string insert_photo = R""""(
INSERT INTO photos (id, owner_id, date)
VALUES (?1, ?2, ?3);
)"""";

    static inline const std::string exists_photo = R""""(
SELECT EXISTS (SELECT * FROM photos WHERE id = ?1);
)"""";

    static inline const std::string insert_video = R""""(
INSERT INTO videos (id, owner_id, date, title, description)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string exists_video = R""""(
SELECT EXISTS (SELECT * FROM videos WHERE id = ?1);
)"""";

    static inline const std::string insert_audio = R""""(
INSERT INTO audios (id, owner_id, artist, title, duration)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string exists_audio = R""""(
SELECT EXISTS (SELECT * FROM audios WHERE id = ?1);
)"""";

    static inline const std::string insert_document = R""""(
INSERT INTO documents (id, owner_id, date, title, ext)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string exists_document = R""""(
SELECT EXISTS (SELECT * FROM documents WHERE id = ?1);
)"""";

    static inline const std::string insert_link = R""""(
INSERT INTO links (id, url, title, caption, description)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string exists_link = R""""(
SELECT EXISTS (SELECT * FROM links WHERE id = ?1);
)"""";

    static inline const std::string insert_product = R""""(
INSERT INTO products (id, owner_id, title, description, price, currency,
category_name, category_section)
VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);
)"""";

    static inline const std::string exists_product = R""""(
SELECT EXISTS (SELECT * FROM products WHERE id = ?1);
)"""";

    static inline const std::string insert_product_album = R""""(
INSERT INTO product_albums (id, owner_id, title, is_main, is_hidden)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string exists_product_album = R""""(
SELECT EXISTS (SELECT * FROM product_albums WHERE id = ?1);
)"""";

    static inline const std::string insert_post = R""""(
INSERT INTO posts (id, owner_id, from_id, date, text)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string exists_post = R""""(
SELECT EXISTS (SELECT * FROM posts WHERE id = ?1);
)"""";

    static inline const std::string insert_comment = R""""(
INSERT INTO comments (id, from_id, date, text)
VALUES (?1, ?2, ?3, ?4);
)"""";

    static inline const std::string exists_comment = R""""(
SELECT EXISTS (SELECT * FROM comments WHERE id = ?1);
)"""";

    static inline const std::string insert_sticker = R""""(
INSERT INTO stickers (id)
VALUES (?1);
)"""";

    static inline const std::string exists_sticker = R""""(
SELECT EXISTS (SELECT * FROM stickers WHERE id = ?1);
)"""";

    static inline const std::string insert_gift = R""""(
INSERT INTO gifts (id)
VALUES (?1);
)"""";

    static inline const std::string exists_gift = R""""(
SELECT EXISTS (SELECT * FROM gifts WHERE id = ?1);
)"""";

    static inline const std::string insert_call = R""""(
INSERT INTO calls (id, initiator_id, receiver_id, state, time, duration,
video)
VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);
)"""";

    static inline const std::string exists_call = R""""(
SELECT EXISTS (SELECT * FROM calls WHERE id = ?1);
)"""";

    static inline const std::string exists_audio_message = R""""(
SELECT EXISTS (SELECT * FROM audio_messages WHERE id = ?1);
)"""";

    static inline const std::string insert_audio_message = R""""(
INSERT INTO audio_messages (id, owner_id, duration, waveform)
VALUES (?1, ?2, ?3, ?4);
)"""";

    static inline const std::string exists_audio_playlist = R""""(
SELECT EXISTS (SELECT * FROM audio_playlists WHERE id = ?1);
)"""";

    static inline const std::string insert_audio_playlist = R""""(
INSERT INTO audio_playlists (id, owner_id, create_time, update_time, year,
title, description)
VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);
)"""";

    static inline const std::string exists_audio_playlist_audio = R""""(
SELECT EXISTS (SELECT * FROM audio_playlist_audios WHERE id = ?1);
)"""";

    static inline const std::string insert_audio_playlist_audio = R""""(
INSERT INTO audio_playlist_audios (audio_id, audio_playlist_id)
VALUES (?1, ?2);
)"""";

    static inline const std::string exists_graffiti = R""""(
SELECT EXISTS (SELECT * FROM graffitis WHERE id = ?1);
)"""";

    static inline const std::string insert_graffiti = R""""(
INSERT INTO graffitis (id, owner_id)
VALUES (?1, ?2);
)"""";

    static inline const std::string exists_money_request = R""""(
SELECT EXISTS (SELECT * FROM money_requests WHERE id = ?1);
)"""";

    static inline const std::string insert_money_request = R""""(
INSERT INTO money_requests (id, from_id, to_id, amount, currency)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

    static inline const std::string exists_story = R""""(
SELECT EXISTS (SELECT * FROM stories WHERE id = ?1);
)"""";

    static inline const std::string insert_story = R""""(
INSERT INTO stories (id, owner_id, date, expires_at)
VALUES (?1, ?2, ?3, ?4);
)"""";

    static inline const std::string insert_message_attachment = R""""(
INSERT INTO message_attachments (message_from_id,
message_conversation_message_id, sequence_number, attachment_id,
attachment_type)
VALUES (?1, ?2, ?3, ?4, ?5);
)"""";

}
