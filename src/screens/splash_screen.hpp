#pragma once

#include <string>

#include <SDL.h>

#include "texture_cache.hpp"

namespace cherry {

// Fullscreen loading splash: the cherry logo centered on black. Purely
// visual, no input handling — matches the original, which shows this for a
// fixed delay while heavier screens redraw.
class SplashScreen {
public:
    SplashScreen(TextureCache& textures, std::string logo_path);

    void render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const;

private:
    TextureCache& textures_;
    std::string logo_path_;
};

}  // namespace cherry
