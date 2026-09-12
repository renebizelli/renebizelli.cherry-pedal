#pragma once

#include <map>
#include <string>

#include <SDL_ttf.h>

namespace cherry {

// Opens and caches one TTF_Font per point size from a single font file,
// since SDL_ttf requires a distinct TTF_Font handle per size.
class FontCache {
public:
    explicit FontCache(std::string font_path);
    ~FontCache();

    FontCache(const FontCache&) = delete;
    FontCache& operator=(const FontCache&) = delete;

    TTF_Font* get(int point_size);

private:
    std::string font_path_;
    std::map<int, TTF_Font*> fonts_;
};

}  // namespace cherry
