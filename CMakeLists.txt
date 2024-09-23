cmake_minimum_required(VERSION 3.27)

set(VK_MESSAGE_EXPORTER_SOURCES
    "src/main.cpp")

project(vk-message-exporter VERSION 0.1.0 LANGUAGES C CXX)

add_executable(vk-message-exporter ${VK_MESSAGE_EXPORTER_SOURCES})
set_target_properties(vk-message-exporter PROPERTIES
    CXX_STANDARD 20)
