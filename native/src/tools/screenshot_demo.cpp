// Manual visual verification tool (not part of the automated test suite):
// renders one screen state against the project's real source.json and
// saves a single PNG, so the UI can be inspected without a physical
// display (SDL's "dummy" video driver still rasterizes into memory) and
// without a real audio device (a trivial no-op AudioPlayer stands in for
// PortAudioPlayer here — this tool only exercises rendering).
//
// One screenshot per process invocation by design: run it once per state
// you want to inspect (see the `mode` argument below).
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "audio_player.hpp"
#include "audio_player_factory.hpp"
#include "font_cache.hpp"
#include "navigator.hpp"
#include "painel_screen.hpp"
#include "sdl_context.hpp"
#include "setup_screen.hpp"
#include "source_service.hpp"
#include "splash_screen.hpp"
#include "texture_cache.hpp"

using namespace cherry;

namespace {

constexpr int kWidth = 800;
constexpr int kHeight = 400;

class NoopNavigator : public Navigator {
public:
    void show_setup() override {}
    void show_panel(const Band&) override {}
    void quit() override {}
};

class NullAudioPlayer : public AudioPlayer {
public:
    void load(const std::string&) override {}
    void play() override { playing_ = true; }
    void stop() override { playing_ = false; }
    bool is_playing() const override { return playing_; }

private:
    bool playing_ = false;
};

class NullAudioPlayerFactory : public AudioPlayerFactory {
public:
    std::unique_ptr<AudioPlayer> create() const override {
        return std::make_unique<NullAudioPlayer>();
    }
};

void save_screenshot(SDL_Renderer* renderer, const std::string& path) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, kWidth, kHeight, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

    if (IMG_SavePNG(surface, path.c_str()) != 0) {
        std::cerr << "IMG_SavePNG failed: " << IMG_GetError() << "\n";
    } else {
        std::cout << "Saved " << path << "\n";
    }

    SDL_FreeSurface(surface);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: screenshot_demo <splash|setup|setup_selected|panel|panel_navigated|panel_playing> <out.png>\n";
        return 1;
    }

    const std::string mode = argv[1];
    const std::string out_path = argv[2];

    SdlContext sdl("Cherry Pedal (demo)", kWidth, kHeight, false);
    FontCache fonts("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
    TextureCache textures(sdl.renderer());
    NoopNavigator navigator;

    SourceService source("../source.json");
    const auto bands = source.bands();

    if (mode == "splash") {
        SplashScreen splash(textures, "../assets/cherry.jpg");
        splash.render(sdl.renderer(), kWidth, kHeight);
    } else if (mode == "setup" || mode == "setup_selected") {
        SetupScreen setup(bands, navigator, fonts, textures, "../assets");
        if (mode == "setup_selected") {
            setup.advance_selection();
        }
        setup.render(sdl.renderer(), kWidth, kHeight);
    } else if (mode == "panel" || mode == "panel_navigated" || mode == "panel_playing") {
        const auto& band = bands.at(1);  // "ask": more songs/audios to look at
        const auto songs = source.songs(band);

        NullAudioPlayerFactory player_factory;
        PainelScreen panel(band, songs, player_factory, navigator, fonts, textures);

        if (mode == "panel_navigated" || mode == "panel_playing") {
            panel.audio_forward();
            panel.song_forward();
        }
        if (mode == "panel_playing") {
            panel.do_play();
        }

        panel.render(sdl.renderer(), kWidth, kHeight);

        SDL_RenderPresent(sdl.renderer());
        save_screenshot(sdl.renderer(), out_path);

        // NullAudioPlayer::is_playing() never turns false on its own (there
        // is no real playback to finish), so PlayerService's background
        // watcher would otherwise loop forever and PainelScreen's
        // destructor would hang joining it. A real backend's is_playing()
        // naturally goes false once the buffer finishes playing.
        panel.do_stop();
        return 0;
    } else {
        std::cerr << "unknown mode: " << mode << "\n";
        return 1;
    }

    SDL_RenderPresent(sdl.renderer());
    save_screenshot(sdl.renderer(), out_path);

    return 0;
}
