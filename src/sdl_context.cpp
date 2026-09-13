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

    const Uint32 window_flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        logical_width,
        logical_height,
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
