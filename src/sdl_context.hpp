#pragma once

#include <string>

#include <SDL.h>

namespace cherry {

// RAII wrapper around SDL2/SDL2_image/SDL2_ttf initialization and the
// window+renderer pair. Renders to a fixed logical canvas (see
// SDL_RenderSetLogicalSize) so screen layout code can use simple absolute
// coordinates regardless of the physical display resolution SDL scales to.
class SdlContext {
public:
    SdlContext(const std::string& title, int logical_width, int logical_height, bool fullscreen);
    ~SdlContext();

    SdlContext(const SdlContext&) = delete;
    SdlContext& operator=(const SdlContext&) = delete;

    SDL_Renderer* renderer() const { return renderer_; }
    SDL_Window* window() const { return window_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
};

}  // namespace cherry
