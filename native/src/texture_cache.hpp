#pragma once

#include <map>
#include <string>

#include <SDL.h>

namespace cherry {

// Loads and caches one SDL_Texture per image path — band logos, the
// selector arrow and the splash logo are each loaded once and reused
// across redraws instead of being decoded every frame.
class TextureCache {
public:
    explicit TextureCache(SDL_Renderer* renderer);
    ~TextureCache();

    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;

    // Returns nullptr if the file does not exist or fails to load — bands
    // without a logo fall back to a text button, matching the original.
    SDL_Texture* try_get(const std::string& path);

private:
    SDL_Renderer* renderer_;
    std::map<std::string, SDL_Texture*> textures_;
};

}  // namespace cherry
