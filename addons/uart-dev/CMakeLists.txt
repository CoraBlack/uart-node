cmake_minimum_required(VERSION 3.20)

project(uart_dev CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GODOTCPP_CUSTOM_API_FILE ${CMAKE_CURRENT_SOURCE_DIR}/extension_api.json)
add_subdirectory(./godot-cpp)

add_library(uart_dev SHARED)

target_sources(uart_dev PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module_register.cpp
)

target_include_directories(uart_dev PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp/gen/include
)

target_link_libraries(uart_dev PRIVATE 
    godot-cpp
)

