#include "font_cache.hpp"

#include <stdexcept>

namespace cherry {

FontCache::FontCache(std::string font_path) : font_path_(std::move(font_path)) {}

FontCache::~FontCache() {
    for (auto& [size, font] : fonts_) {
        TTF_CloseFont(font);
    }
}

TTF_Font* FontCache::get(int point_size) {
    auto it = fonts_.find(point_size);
    if (it != fonts_.end()) {
        return it->second;
    }

    TTF_Font* font = TTF_OpenFont(font_path_.c_str(), point_size);
    if (font == nullptr) {
        throw std::runtime_error(std::string("Could not open font: ") + TTF_GetError());
    }

    fonts_[point_size] = font;
    return font;
}

}  // namespace cherry
