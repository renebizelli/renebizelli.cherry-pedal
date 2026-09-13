#include <iostream>
#include <memory>
#include <string>

#include <unistd.h>

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

int main(int argc, char** argv) {
    std::unique_ptr<SourceService> source;

    try {
        source = std::make_unique<SourceService>("source.json");
        source->validate_audio_files();
    } catch (const std::exception& error) {
        show_fatal_error(error.what());
        return 1;
    }

    bool restart_requested = false;

    try {
        SdlContext sdl("Cherry", kCanvasWidth, kCanvasHeight, /*fullscreen=*/true);
        FontCache fonts(kFontPath);
        TextureCache textures(sdl.renderer());

        auto channel = std::make_shared<PortAudioChannel>();
        PortAudioPlayerFactory player_factory(channel);
        GpiodButtonFactory button_factory;

        Application app(*source, player_factory, button_factory, fonts, textures, "assets");
        app.run(sdl.renderer(), kCanvasWidth, kCanvasHeight);
        restart_requested = app.restart_requested();
    } catch (const std::exception& error) {
        show_fatal_error(error.what());
        return 1;
    }

    if (restart_requested) {
        // Reloads source.json/bands/ from scratch after a content import
        // (SyncScreen) — re-executing the whole process is simpler and more
        // robust than adding a separate "hot reload" code path alongside the
        // one already exercised on every normal boot.
        (void)argc;
        execv("/proc/self/exe", argv);
        std::cerr << "Falha ao reiniciar automaticamente apos importar conteudo novo.\n";
        return 1;
    }

    return 0;
}
