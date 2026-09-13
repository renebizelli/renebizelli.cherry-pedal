#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <SDL.h>

#include "font_cache.hpp"
#include "models.hpp"
#include "navigator.hpp"
#include "texture_cache.hpp"

namespace cherry {

// Band selection screen: a vertical list of band logos/buttons with a
// selector arrow, navigated by advancing the selection and confirming it.
// advance_selection()/confirm_selection()/exit_app() are the same handlers
// both keyboard input and (in the composition phase) GPIO input call —
// mirroring how the original bound the same methods to both input sources.
class SetupScreen {
public:
    SetupScreen(
        std::vector<Band> bands,
        Navigator& navigator,
        FontCache& fonts,
        TextureCache& textures,
        std::string assets_dir);

    void reset_selection();
    void advance_selection();
    void confirm_selection();
    void exit_app();

    void handle_key(SDL_Keycode key);
    // Selects the band under (x, y) or triggers the exit button, matching
    // the original's direct click-to-select (mouse isn't otherwise used —
    // the pedal is normally operated by footswitch/keyboard).
    void handle_click(int x, int y, int canvas_width, int canvas_height);
    void render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const;

private:
    void render_band(SDL_Renderer* renderer, const SDL_Rect& row, const Band& band, bool selected) const;
    SDL_Rect band_row_rect(std::size_t index, int canvas_height) const;
    SDL_Rect exit_button_rect(int canvas_height) const;

    std::vector<Band> bands_;
    Navigator& navigator_;
    FontCache& fonts_;
    TextureCache& textures_;
    std::string assets_dir_;
    std::size_t index_selected_ = 0;
};

}  // namespace cherry
