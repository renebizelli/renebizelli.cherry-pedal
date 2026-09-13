#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <SDL.h>

#include "content_importer.hpp"
#include "media_scanner.hpp"
#include "models.hpp"
#include "navigator.hpp"
#include "font_cache.hpp"
#include "texture_cache.hpp"

namespace cherry {

// Content sync screen: lists removable-media "cherry-pedal-data" packages
// (see MediaScanner) and imports the one the user taps (see
// ContentImporter). Touch/click only — this is a maintenance screen, not
// part of the live-performance input path, so it doesn't need
// keyboard/GPIO parity with the other screens.
class SyncScreen {
public:
    SyncScreen(
        Band origin_band,
        std::filesystem::path project_root,
        Navigator& navigator,
        FontCache& fonts,
        TextureCache& textures);

    void handle_click(int x, int y, int canvas_width, int canvas_height);
    void render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const;

private:
    void rescan();

    Band origin_band_;
    std::filesystem::path project_root_;
    Navigator& navigator_;
    FontCache& fonts_;
    TextureCache& textures_;

    MediaScanner scanner_;
    ContentImporter importer_;
    std::vector<std::filesystem::path> candidates_;
    std::string status_message_;
};

}  // namespace cherry
