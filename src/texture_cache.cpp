#include "texture_cache.hpp"

#include <filesystem>

#include <SDL_image.h>

namespace cherry {

TextureCache::TextureCache(SDL_Renderer* renderer) : renderer_(renderer) {}

TextureCache::~TextureCache() {
    for (auto& [path, texture] : textures_) {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
        }
    }
}

SDL_Texture* TextureCache::try_get(const std::string& path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second;
    }

    SDL_Texture* texture = nullptr;
    if (std::filesystem::is_regular_file(path)) {
        texture = IMG_LoadTexture(renderer_, path.c_str());
    }

    // Cached even when null, so a missing/broken file isn't retried every frame.
    textures_[path] = texture;
    return texture;
}

}  // namespace cherry
