#pragma once

namespace vme::db::sql
{

    static inline const char* init = R""""(
CREATE TABLE IF NOT EXISTS messages (
    from_id INTEGER NOT NULL,
    conversation_message_id INTEGER NOT NULL,
    date INTEGER NOT NULL,
    important INTEGER NOT NULL,
    text TEXT NOT NULL,
    reply_conversation_message_id INTEGER,
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
    date INTEGER NOT NULL,
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

}
