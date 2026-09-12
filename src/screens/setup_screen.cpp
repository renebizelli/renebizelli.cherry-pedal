#include "setup_screen.hpp"

#include "text_renderer.hpp"

namespace cherry {

namespace {
constexpr int kSelectorColumnWidth = 200;
constexpr int kBandColumnWidth = 400;
constexpr int kExitButtonHeight = 50;
constexpr int kSelectorSize = 50;
}

SetupScreen::SetupScreen(
    std::vector<Band> bands,
    Navigator& navigator,
    FontCache& fonts,
    TextureCache& textures,
    std::string assets_dir)
    : bands_(std::move(bands)),
      navigator_(navigator),
      fonts_(fonts),
      textures_(textures),
      assets_dir_(std::move(assets_dir)) {}

void SetupScreen::reset_selection() {
    index_selected_ = 0;
}

void SetupScreen::advance_selection() {
    if (bands_.empty()) {
        return;
    }

    index_selected_ = (index_selected_ + 1) % bands_.size();
}

void SetupScreen::confirm_selection() {
    if (bands_.empty()) {
        return;
    }

    navigator_.show_panel(bands_[index_selected_]);
}

void SetupScreen::exit_app() {
    navigator_.quit();
}

void SetupScreen::handle_key(SDL_Keycode key) {
    switch (key) {
        case SDLK_RIGHT:
            advance_selection();
            break;
        case SDLK_SPACE:
            confirm_selection();
            break;
        default:
            break;
    }
}

void SetupScreen::render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (bands_.empty()) {
        return;
    }

    const int row_height = (canvas_height - kExitButtonHeight) / static_cast<int>(bands_.size());

    for (std::size_t i = 0; i < bands_.size(); ++i) {
        const SDL_Rect row{
            kSelectorColumnWidth,
            static_cast<int>(i) * row_height,
            kBandColumnWidth,
            row_height,
        };
        const bool selected = (i == index_selected_);
        render_band(renderer, row, bands_[i], selected);

        if (selected) {
            SDL_Texture* selector = textures_.try_get(assets_dir_ + "/selector.jpg");
            if (selector != nullptr) {
                const SDL_Rect selector_rect{
                    (kSelectorColumnWidth - kSelectorSize) / 2,
                    row.y + (row_height - kSelectorSize) / 2,
                    kSelectorSize,
                    kSelectorSize,
                };
                SDL_RenderCopy(renderer, selector, nullptr, &selector_rect);
            }
        }
    }

    const SDL_Rect exit_rect{
        kSelectorColumnWidth,
        canvas_height - kExitButtonHeight,
        kBandColumnWidth,
        kExitButtonHeight,
    };
    SDL_SetRenderDrawColor(renderer, 139, 0, 0, 255);  // darkred
    SDL_RenderFillRect(renderer, &exit_rect);
    render_text(
        renderer, fonts_, "SAIR", exit_rect, TextStyle{18, true, SDL_Color{255, 255, 255, 255}});
}

void SetupScreen::render_band(
    SDL_Renderer* renderer, const SDL_Rect& row, const Band& band, bool selected) const {
    (void)selected;
    const std::string logo_path = assets_dir_ + "/bands/" + band.id + ".jpg";
    SDL_Texture* logo = band.logo ? textures_.try_get(logo_path) : nullptr;

    if (logo != nullptr) {
        int texture_width = 0;
        int texture_height = 0;
        SDL_QueryTexture(logo, nullptr, nullptr, &texture_width, &texture_height);

        const int display_height = row.h - 10;
        const int display_width = (texture_height == 0)
            ? display_height
            : texture_width * display_height / texture_height;

        const SDL_Rect dest{
            row.x + (row.w - display_width) / 2,
            row.y + (row.h - display_height) / 2,
            display_width,
            display_height,
        };
        SDL_RenderCopy(renderer, logo, nullptr, &dest);
    } else {
        render_text(
            renderer, fonts_, band.name, row, TextStyle{22, false, SDL_Color{255, 255, 255, 255}});
    }
}

}  // namespace cherry
