#include <iostream>
#include <memory>
#include <string>

#include <SDL.h>

#include "application.hpp"
#include "font_cache.hpp"
#include "gpiod_button_factory.hpp"
#include "portaudio_channel.hpp"
#include "portaudio_player_factory.hpp"
#include "sdl_context.hpp"
#include "source_service.hpp"
#include "texture_cache.hpp"

using namespace cherry;

namespace {

constexpr int kCanvasWidth = 800;
constexpr int kCanvasHeight = 400;
constexpr const char* kFontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";

void show_fatal_error(const std::string& message) {
    std::cerr << "Erro ao iniciar o Cherry: " << message << "\n";
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Cherry", message.c_str(), nullptr);
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

    try {
        SdlContext sdl("Cherry", kCanvasWidth, kCanvasHeight, /*fullscreen=*/true);
        FontCache fonts(kFontPath);
        TextureCache textures(sdl.renderer());

        auto channel = std::make_shared<PortAudioChannel>();
        PortAudioPlayerFactory player_factory(channel);
        GpiodButtonFactory button_factory;

        Application app(*source, player_factory, button_factory, fonts, textures, "assets");
        app.run(sdl.renderer(), kCanvasWidth, kCanvasHeight);
    } catch (const std::exception& error) {
        show_fatal_error(error.what());
        return 1;
    }

    return 0;
}
