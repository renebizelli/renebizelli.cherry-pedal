cmake_minimum_required(VERSION 3.16)
project(cherry_pedal_native CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    # Our own main() is the real entry point; without this, SDL.h redefines
    # main to SDL_main and expects to link against a separate SDL2main lib
    # (and a WinMain shim) that the UI-only test build doesn't need.
    add_compile_definitions(SDL_MAIN_HANDLED)
endif()

# UI-only builds (e.g. cross-compiled to Windows with MinGW for a quick
# visual test via screenshot_demo) have no PortAudio/libsndfile/libgpiod
# packages available and don't need them: neither the real audio backend
# nor the real GPIO backend nor the full cherry_pedal executable are built.
option(CHERRY_UI_ONLY "Skip the PortAudio/libgpiod backends and the cherry_pedal executable" OFF)

find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
if(NOT CHERRY_UI_ONLY)
    pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
    pkg_check_modules(SNDFILE REQUIRED sndfile)
    pkg_check_modules(GPIOD REQUIRED libgpiod)
endif()
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# 32-bit ARM (armhf, e.g. a 32-bit Raspberry Pi OS userland) has no native
# CPU instruction for 8-byte atomic load/store, so std::atomic<int64_t>
# needs libatomic explicitly linked; 64-bit targets don't need it, but
# linking it there is harmless. Passed as a plain "-latomic" (not via
# find_library, which doesn't search the compiler's own internal library
# directories where libatomic.so actually lives) so the compiler driver
# resolves it the same way it would for any other implicit runtime lib.

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
    src/resampler.cpp
    src/media_scanner.cpp
    src/content_importer.cpp
)
target_include_directories(cherry_core PUBLIC src)
target_link_libraries(cherry_core PUBLIC nlohmann_json::nlohmann_json Threads::Threads)

if(NOT CHERRY_UI_ONLY)
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
endif()

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
    src/screens/sync_screen.cpp
)
target_include_directories(cherry_ui PUBLIC src src/screens ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
target_link_directories(cherry_ui PUBLIC ${SDL2_LIBRARY_DIRS} ${SDL2_IMAGE_LIBRARY_DIRS} ${SDL2_TTF_LIBRARY_DIRS})
target_link_libraries(cherry_ui PUBLIC cherry_core ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
if(NOT WIN32)
    # 32-bit ARM has no native 8-byte atomic instruction (see note above);
    # MinGW/Windows builds don't need libatomic and may not have it available.
    target_link_libraries(cherry_ui PUBLIC atomic)
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

if(CHERRY_UI_ONLY)
    # Interactive window for UI-only test builds (e.g. Windows/MinGW), with
    # no GPIO/audio hardware available — see desktop_demo.cpp.
    add_executable(desktop_demo
        src/application.cpp
        src/tools/desktop_demo.cpp
    )
    target_link_libraries(desktop_demo PRIVATE cherry_ui)
endif()

add_executable(test_action_queue src/tests/test_action_queue.cpp)
target_link_libraries(test_action_queue PRIVATE cherry_core)

add_executable(test_resampler src/tests/test_resampler.cpp)
target_link_libraries(test_resampler PRIVATE cherry_core)

add_executable(test_media_scanner src/tests/test_media_scanner.cpp)
target_link_libraries(test_media_scanner PRIVATE cherry_core)

add_executable(test_content_importer src/tests/test_content_importer.cpp)
target_link_libraries(test_content_importer PRIVATE cherry_core)

if(NOT CHERRY_UI_ONLY)
    # Final composed application: real SDL2 UI + real PortAudio + real libgpiod.
    # Like each hardware backend individually, end-to-end behavior can only be
    # fully verified on the Raspberry Pi.
    add_executable(cherry_pedal
        src/application.cpp
        src/main.cpp
    )
    target_link_libraries(cherry_pedal PRIVATE cherry_ui cherry_audio_backend cherry_gpio_backend)
endif()
