#include "splash_screen.hpp"

namespace cherry {

namespace {
constexpr int kLogoWidth = 150;
constexpr int kLogoHeight = 350;
}

SplashScreen::SplashScreen(TextureCache& textures, std::string logo_path)
    : textures_(textures), logo_path_(std::move(logo_path)) {}

void SplashScreen::render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* logo = textures_.try_get(logo_path_);
    if (logo == nullptr) {
        return;
    }

    const SDL_Rect dest{
        (canvas_width - kLogoWidth) / 2,
        (canvas_height - kLogoHeight) / 2,
        kLogoWidth,
        kLogoHeight,
    };

    SDL_RenderCopy(renderer, logo, nullptr, &dest);
}

}  // namespace cherry
