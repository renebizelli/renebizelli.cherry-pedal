#include "sdl_context.hpp"

#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>

namespace cherry {

SdlContext::SdlContext(const std::string& title, int logical_width, int logical_height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    if ((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) == 0) {
        throw std::runtime_error(std::string("IMG_Init failed: ") + IMG_GetError());
    }

    if (TTF_Init() != 0) {
        throw std::runtime_error(std::string("TTF_Init failed: ") + TTF_GetError());
    }

    // SDL_WINDOW_FULLSCREEN_DESKTOP is supposed to make SDL ignore the
    // requested width/height and stretch the window to the desktop
    // resolution itself, but that relies on the window manager honoring the
    // fullscreen state hint. On this device's Openbox/PIXEL setup it
    // doesn't: the window comes up at the literal logical size requested
    // (e.g. 800x400) centered inside the real, larger physical resolution
    // (e.g. 800x480), leaving real empty margins above/below where touches
    // never reach the window at all (mistaken, during testing, for a touch
    // calibration problem). Asking for the actual current display mode's
    // size up front avoids that regardless of the window manager's fullscreen
    // support; SDL_RenderSetLogicalSize below still letterboxes/scales the
    // fixed logical canvas into whatever size the window ends up being.
    int window_width = logical_width;
    int window_height = logical_height;
    if (fullscreen) {
        SDL_DisplayMode display_mode;
        if (SDL_GetCurrentDisplayMode(0, &display_mode) == 0) {
            window_width = display_mode.w;
            window_height = display_mode.h;
        }
    }

    const Uint32 window_flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        window_flags);

    if (window_ == nullptr) {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) {
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    }
    if (renderer_ == nullptr) {
        throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    }

    SDL_RenderSetLogicalSize(renderer_, logical_width, logical_height);
}

SdlContext::~SdlContext() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
    }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

}  // namespace cherry
