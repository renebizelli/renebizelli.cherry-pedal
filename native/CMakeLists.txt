cmake_minimum_required(VERSION 3.16)
project(cherry_pedal_native CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)

add_library(cherry_core
    src/json_file_reader.cpp
    src/path_resolver.cpp
    src/audio_file_validator.cpp
    src/source_service.cpp
)
target_include_directories(cherry_core PUBLIC src)
target_link_libraries(cherry_core PUBLIC nlohmann_json::nlohmann_json)

add_executable(test_source_service src/tests/test_source_service.cpp)
target_link_libraries(test_source_service PRIVATE cherry_core)
