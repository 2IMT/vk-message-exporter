cmake_minimum_required(VERSION 3.27)

set(VK_MESSAGE_EXPORTER_SOURCES
    "src/main.cpp"
    "src/error.h"
    "src/args.h"
    "src/args.cpp"
    "src/curl.h"
    "src/curl.cpp"
    "src/db/db.h"
    "src/db/db.cpp"
    "src/db/sql.h"
    "src/db/storage.h"
    "src/db/storage.cpp"
    "src/api/session.h"
    "src/api/session.cpp"
    "src/api/params.h"
    "src/api/params.cpp"
    "src/api/vk_data.h"
    "src/api/vk_data.cpp"
    "src/api/message_stream.h"
    "src/api/message_stream.cpp"
)

project(vk-message-exporter VERSION 0.1.0 LANGUAGES C CXX)

add_executable(vk-message-exporter ${VK_MESSAGE_EXPORTER_SOURCES})
set_target_properties(vk-message-exporter PROPERTIES
    CXX_STANDARD 20)
target_include_directories(vk-message-exporter PRIVATE "src")

find_package(SQLite3 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CURL REQUIRED)

target_link_libraries(vk-message-exporter 
    ${SQLite3_LIBRARIES} 
    nlohmann_json::nlohmann_json
    ${CURL_LIBRARIES}
)
