// Interactive desktop test build (CHERRY_UI_ONLY, e.g. the MinGW/Windows
// cross-build): opens a real window and drives the full Application exactly
// like the Raspberry Pi build — same screens, same navigation — but with no
// GPIO and no audio device available on a dev machine. Footswitches are
// unreachable (nothing ever presses the null GPIO source), so use the
// keyboard bindings already in PainelScreen::handle_key /
// SetupScreen::handle_key instead; audio is silently no-op'd rather than
// played. Unlike screenshot_demo (one static frame per run), this keeps a
// real event loop running so screens can be exercised interactively.
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <SDL.h>

#include "application.hpp"
#include "audio_player.hpp"
#include "audio_player_factory.hpp"
#include "debounced_button.hpp"
#include "font_cache.hpp"
#include "gpio_button_factory.hpp"
#include "gpio_event_source.hpp"
#include "sdl_context.hpp"
#include "source_service.hpp"
#include "texture_cache.hpp"

using namespace cherry;

namespace {

constexpr int kCanvasWidth = 800;
constexpr int kCanvasHeight = 400;

void show_fatal_error(const std::string& message) {
    std::cerr << "Erro ao iniciar o Cherry (desktop test): " << message << "\n";
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR, "Cherry (desktop test)", message.c_str(), nullptr);
}

// Never reports a press: on a dev machine there are no real footswitches to
// wire up, so GPIO-bound actions are simply unreachable via this source.
class NullGpioEventSource : public GpioEventSource {
public:
    bool wait_for_edge(std::chrono::milliseconds timeout) override {
        SDL_Delay(static_cast<Uint32>(timeout.count()));
        return false;
    }
};

class NullGpioButtonFactory : public GpioButtonFactory {
public:
    std::unique_ptr<DebouncedButton> create(
        unsigned int /*pin*/, DebouncedButton::PressCallback on_press) const override {
        return std::make_unique<DebouncedButton>(
            std::make_unique<NullGpioEventSource>(),
            std::chrono::milliseconds(0),
            std::move(on_press));
    }
};

// Simulates a clip of fixed length elapsing in real time (there is no real
// decoded audio to derive a duration/position from), so play/stop and the
// progress bar in PainelScreen can be exercised end-to-end interactively
// instead of always reporting 0% progress and never finishing on its own.
class NullAudioPlayer : public AudioPlayer {
public:
    void load(const std::string&) override {}

    void play() override {
        playing_ = true;
        started_at_ = std::chrono::steady_clock::now();
    }

    void stop() override { playing_ = false; }

    bool is_playing() const override {
        return playing_ && elapsed_fraction() < 1.0;
    }

    double progress() const override {
        return playing_ ? std::min(elapsed_fraction(), 1.0) : 0.0;
    }

private:
    static constexpr auto kFakeDuration = std::chrono::seconds(6);

    double elapsed_fraction() const {
        const auto elapsed = std::chrono::steady_clock::now() - started_at_;
        return std::chrono::duration<double>(elapsed) / kFakeDuration;
    }

    bool playing_ = false;
    std::chrono::steady_clock::time_point started_at_;
};

class NullAudioPlayerFactory : public AudioPlayerFactory {
public:
    std::unique_ptr<AudioPlayer> create() const override {
        return std::make_unique<NullAudioPlayer>();
    }
};

std::string font_path() {
#ifdef _WIN32
    // Resolved next to the executable itself (not cwd) so this still finds
    // the bundled font regardless of the working directory it's launched
    // from — see the matching comment in screenshot_demo.cpp.
    char* base_path_raw = SDL_GetBasePath();
    std::string path =
        std::string(base_path_raw != nullptr ? base_path_raw : "") + "DejaVuSans-Bold.ttf";
    SDL_free(base_path_raw);
    return path;
#else
    return "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";
#endif
}

}  // namespace

int main() {
    std::unique_ptr<SourceService> source;

    try {
        source = std::make_unique<SourceService>("source.json");
        source->validate_audio_files();
    } catch (const std::exception& error) {
        show_fatal_error(error.what());
        return 1;
    }

    // Mirrors main.cpp's restart-after-content-import flow, but by looping
    // in-process instead of execv() (Linux-only, and unneeded here: nothing
    // about this process's own state needs a hard reset for a UI test).
    bool restart_requested = true;
    while (restart_requested) {
        restart_requested = false;

        try {
            SdlContext sdl(
                "Cherry (desktop test - sem audio/GPIO real)", kCanvasWidth, kCanvasHeight, false);
            FontCache fonts(font_path());
            TextureCache textures(sdl.renderer());

            NullAudioPlayerFactory player_factory;
            NullGpioButtonFactory button_factory;

            Application app(*source, player_factory, button_factory, fonts, textures, "assets");
            app.run(sdl.renderer(), kCanvasWidth, kCanvasHeight);
            restart_requested = app.restart_requested();

            if (restart_requested) {
                source = std::make_unique<SourceService>("source.json");
                source->validate_audio_files();
            }
        } catch (const std::exception& error) {
            show_fatal_error(error.what());
            return 1;
        }
    }

    return 0;
}
