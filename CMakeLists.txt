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
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# 32-bit ARM (armhf, e.g. a 32-bit Raspberry Pi OS userland) has no native
# CPU instruction for 8-byte atomic load/store, so std::atomic<int64_t>
# needs libatomic explicitly linked; 64-bit targets don't need it but
# linking it there is harmless.
find_library(ATOMIC_LIBRARY atomic)

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
    src/layout_helpers.cpp
    src/action_queue.cpp
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

# UI layer (SDL2 + SDL2_image + SDL2_ttf). Screens open a real
# window/renderer, so — like the audio and GPIO backends — end-to-end
# behavior can only be fully confirmed on the Raspberry Pi; the pure
# layout heuristics (layout_helpers, in cherry_core) are unit-tested
# directly, and the screenshot_demo tool below renders each screen against
# SDL's headless "dummy" video driver for a quick visual check without a
# physical display.
add_library(cherry_ui
    src/sdl_context.cpp
    src/font_cache.cpp
    src/texture_cache.cpp
    src/text_renderer.cpp
    src/screens/splash_screen.cpp
    src/screens/setup_screen.cpp
    src/screens/painel_screen.cpp
)
target_include_directories(cherry_ui PUBLIC src src/screens ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
target_link_directories(cherry_ui PUBLIC ${SDL2_LIBRARY_DIRS} ${SDL2_IMAGE_LIBRARY_DIRS} ${SDL2_TTF_LIBRARY_DIRS})
target_link_libraries(cherry_ui PUBLIC cherry_core ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
if(ATOMIC_LIBRARY)
    target_link_libraries(cherry_ui PUBLIC ${ATOMIC_LIBRARY})
endif()

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

add_executable(test_layout_helpers src/tests/test_layout_helpers.cpp)
target_link_libraries(test_layout_helpers PRIVATE cherry_core)

add_executable(screenshot_demo src/tools/screenshot_demo.cpp)
target_link_libraries(screenshot_demo PRIVATE cherry_ui)

add_executable(test_action_queue src/tests/test_action_queue.cpp)
target_link_libraries(test_action_queue PRIVATE cherry_core)

# Final composed application: real SDL2 UI + real PortAudio + real libgpiod.
# Like each hardware backend individually, end-to-end behavior can only be
# fully verified on the Raspberry Pi.
add_executable(cherry_pedal
    src/application.cpp
    src/main.cpp
)
target_link_libraries(cherry_pedal PRIVATE cherry_ui cherry_audio_backend cherry_gpio_backend)
