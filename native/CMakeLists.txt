cmake_minimum_required(VERSION 3.16)
project(cherry_pedal_native CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
pkg_check_modules(SNDFILE REQUIRED sndfile)
pkg_check_modules(GPIOD REQUIRED libgpiod)

add_library(cherry_core
    src/json_file_reader.cpp
    src/path_resolver.cpp
    src/audio_file_validator.cpp
    src/source_service.cpp
    src/playback_sequencer.cpp
    src/player_service.cpp
    src/audio_service.cpp
    src/song_service.cpp
    src/pedal_controller.cpp
    src/debounced_button.cpp
    src/input_service.cpp
)
target_include_directories(cherry_core PUBLIC src)
target_link_libraries(cherry_core PUBLIC nlohmann_json::nlohmann_json Threads::Threads)

# Real PortAudio/libsndfile backend. Only compiled here (no automated test
# runs it) — it opens an actual audio device, so correctness can only be
# verified on the Raspberry Pi or another machine with real audio hardware.
add_library(cherry_audio_backend
    src/wav_decoder.cpp
    src/portaudio_channel.cpp
    src/portaudio_player.cpp
)
target_include_directories(cherry_audio_backend PUBLIC src ${PORTAUDIO_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS})
target_link_directories(cherry_audio_backend PUBLIC ${PORTAUDIO_LIBRARY_DIRS} ${SNDFILE_LIBRARY_DIRS})
target_link_libraries(cherry_audio_backend PUBLIC cherry_core ${PORTAUDIO_LIBRARIES} ${SNDFILE_LIBRARIES})

# Real libgpiod backend. No automated test exercises it — it opens a real
# GPIO chip, so correctness can only be verified on the Raspberry Pi with
# actual footswitches attached (matches the PRD's own test requirements).
add_library(cherry_gpio_backend
    src/gpiod_event_source.cpp
    src/gpiod_button_factory.cpp
)
target_include_directories(cherry_gpio_backend PUBLIC src ${GPIOD_INCLUDE_DIRS})
target_link_directories(cherry_gpio_backend PUBLIC ${GPIOD_LIBRARY_DIRS})
target_link_libraries(cherry_gpio_backend PUBLIC cherry_core ${GPIOD_LIBRARIES})

add_executable(test_source_service src/tests/test_source_service.cpp)
target_link_libraries(test_source_service PRIVATE cherry_core)

add_executable(test_player_service src/tests/test_player_service.cpp)
target_link_libraries(test_player_service PRIVATE cherry_core)

add_executable(test_audio_service src/tests/test_audio_service.cpp)
target_link_libraries(test_audio_service PRIVATE cherry_core)

add_executable(test_debounced_button src/tests/test_debounced_button.cpp)
target_link_libraries(test_debounced_button PRIVATE cherry_core)

add_executable(test_input_service src/tests/test_input_service.cpp)
target_link_libraries(test_input_service PRIVATE cherry_core)
