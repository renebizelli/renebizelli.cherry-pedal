#include "text_renderer.hpp"

#include <SDL_ttf.h>

namespace cherry {

SDL_Rect render_text(
    SDL_Renderer* renderer,
    FontCache& fonts,
    const std::string& text,
    const SDL_Rect& bounds,
    const TextStyle& style) {
    if (text.empty()) {
        return SDL_Rect{bounds.x, bounds.y, 0, 0};
    }

    TTF_Font* font = fonts.get(style.point_size);
    TTF_SetFontStyle(font, style.bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), style.color);
    if (surface == nullptr) {
        return SDL_Rect{bounds.x, bounds.y, 0, 0};
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    const int text_width = surface->w;
    const int text_height = surface->h;
    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        return SDL_Rect{bounds.x, bounds.y, 0, 0};
    }

    int x = bounds.x;
    if (style.align == TextAlign::Center) {
        x = bounds.x + (bounds.w - text_width) / 2;
    } else if (style.align == TextAlign::Right) {
        x = bounds.x + bounds.w - text_width;
    }

    const int y = bounds.y + (bounds.h - text_height) / 2;
    const SDL_Rect dest{x, y, text_width, text_height};

    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_DestroyTexture(texture);

    return dest;
}

}  // namespace cherry
