#pragma once

#include <string>

#include <SDL.h>

#include "font_cache.hpp"

namespace cherry {

enum class TextAlign { Left, Center, Right };

struct TextStyle {
    int point_size;
    bool bold;
    SDL_Color color;
    TextAlign align = TextAlign::Center;
};

// Renders `text` inside `bounds`, vertically centered and horizontally
// aligned per style.align. Returns the pixel rect actually drawn (mostly
// useful for tests/tools); does nothing (and returns an empty rect) for
// empty text.
SDL_Rect render_text(
    SDL_Renderer* renderer,
    FontCache& fonts,
    const std::string& text,
    const SDL_Rect& bounds,
    const TextStyle& style);

}  // namespace cherry
