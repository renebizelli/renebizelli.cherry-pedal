#include "splash_screen.hpp"

#include <algorithm>

namespace cherry {

namespace {
constexpr int kLogoWidth = 150;
constexpr int kLogoHeight = 350;
constexpr int kBarWidth = 200;
constexpr int kBarHeight = 6;
constexpr int kBarMarginTop = 14;
// The logo alone (350px tall) barely fits the 400px-tall canvas already —
// centering the bar below it too means centering the whole logo+gap+bar
// block, not just the logo, or the bar would run off the bottom edge.
constexpr int kContentHeight = kLogoHeight + kBarMarginTop + kBarHeight;
}

SplashScreen::SplashScreen(TextureCache& textures, std::string logo_path)
    : textures_(textures), logo_path_(std::move(logo_path)) {}

void SplashScreen::render(
    SDL_Renderer* renderer, int canvas_width, int canvas_height, double progress) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* logo = textures_.try_get(logo_path_);
    if (logo == nullptr) {
        return;
    }

    const int logo_top = (canvas_height - kContentHeight) / 2;
    const SDL_Rect dest{
        (canvas_width - kLogoWidth) / 2,
        logo_top,
        kLogoWidth,
        kLogoHeight,
    };

    SDL_RenderCopy(renderer, logo, nullptr, &dest);

    const double clamped_progress = std::clamp(progress, 0.0, 1.0);
    const SDL_Rect bar_track{
        (canvas_width - kBarWidth) / 2,
        logo_top + kLogoHeight + kBarMarginTop,
        kBarWidth,
        kBarHeight,
    };
    const SDL_Rect bar_fill{
        bar_track.x,
        bar_track.y,
        static_cast<int>(kBarWidth * clamped_progress),
        kBarHeight,
    };

    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &bar_track);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &bar_fill);
}

}  // namespace cherry
