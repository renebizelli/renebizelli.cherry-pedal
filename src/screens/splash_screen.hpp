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

    // progress in [0.0, 1.0] fills a white bar under the logo; values
    // outside that range are clamped. Defaults to a full bar, matching a
    // caller with nothing incremental to report (there's no partial state
    // worth showing, so "done" reads better than an empty bar).
    void render(
        SDL_Renderer* renderer, int canvas_width, int canvas_height, double progress = 1.0) const;

private:
    TextureCache& textures_;
    std::string logo_path_;
};

}  // namespace cherry
