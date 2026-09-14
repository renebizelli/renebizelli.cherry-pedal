#include "sync_screen.hpp"

#include "text_renderer.hpp"

namespace cherry {

namespace {
constexpr int kHeaderHeight = 70;
constexpr int kRowHeight = 60;
constexpr int kPadding = 16;
}  // namespace

SyncScreen::SyncScreen(
    Band origin_band,
    std::filesystem::path project_root,
    Navigator& navigator,
    FontCache& fonts,
    TextureCache& textures)
    : origin_band_(std::move(origin_band)),
      project_root_(std::move(project_root)),
      navigator_(navigator),
      fonts_(fonts),
      textures_(textures) {
    rescan();
}

void SyncScreen::rescan() {
    candidates_ = scanner_.find_candidates();
}

void SyncScreen::handle_click(int x, int y, int canvas_width, int canvas_height) {
    (void)canvas_width;
    (void)canvas_height;

    // Generous header band on the top side only (see PainelScreen's own
    // click zone for why: real touchscreen accuracy near the top edge
    // proved unreliable in testing). Capped at kHeaderHeight exactly (no
    // extra slack below it) so it can't swallow taps meant for the first
    // candidate/rescan row, which starts right below the header.
    if (y >= -80 && y <= kHeaderHeight) {
        navigator_.show_panel(origin_band_);
        return;
    }

    if (candidates_.empty()) {
        const SDL_Rect rescan_rect{kPadding, kHeaderHeight + 40, 300, 60};
        const SDL_Point point{x, y};
        if (SDL_PointInRect(&point, &rescan_rect)) {
            rescan();
        }
        return;
    }

    for (std::size_t i = 0; i < candidates_.size(); ++i) {
        const SDL_Rect row{
            0, kHeaderHeight + static_cast<int>(i) * kRowHeight, 800, kRowHeight};
        const SDL_Point point{x, y};
        if (SDL_PointInRect(&point, &row)) {
            try {
                importer_.import_from(candidates_[i], project_root_);
                navigator_.request_restart();
            } catch (const ContentImportError& error) {
                status_message_ = error.what();
                // A small status line at the bottom of a touchscreen kiosk
                // is easy to miss (e.g. someone walking away mid-copy) — a
                // blocking popup makes the failure impossible to overlook.
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR, "Erro ao sincronizar", error.what(), nullptr);
                rescan();
            }
            return;
        }
    }
}

void SyncScreen::render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const SDL_Rect header_rect{kPadding, 0, canvas_width - 2 * kPadding, kHeaderHeight};
    render_text(
        renderer,
        fonts_,
        "< Voltar  |  Sincronizar conteudo",
        header_rect,
        TextStyle{26, true, SDL_Color{255, 255, 255, 255}, TextAlign::Left});

    if (candidates_.empty()) {
        const SDL_Rect message_rect{
            kPadding, kHeaderHeight + 10, canvas_width - 2 * kPadding, 30};
        render_text(
            renderer,
            fonts_,
            "Nenhum pendrive com dados encontrado.",
            message_rect,
            TextStyle{20, false, SDL_Color{255, 255, 0, 255}, TextAlign::Left});

        const SDL_Rect rescan_rect{kPadding, kHeaderHeight + 40, 300, 60};
        SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255);  // darkblue
        SDL_RenderFillRect(renderer, &rescan_rect);
        render_text(
            renderer,
            fonts_,
            "Verificar novamente",
            rescan_rect,
            TextStyle{20, true, SDL_Color{255, 255, 255, 255}});
    } else {
        for (std::size_t i = 0; i < candidates_.size(); ++i) {
            const SDL_Rect row{
                0, kHeaderHeight + static_cast<int>(i) * kRowHeight, canvas_width, kRowHeight};
            const SDL_Rect label_rect{
                row.x + kPadding, row.y, row.w - 2 * kPadding, row.h};

            SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255);  // darkblue
            const SDL_Rect inset{row.x + 4, row.y + 4, row.w - 8, row.h - 8};
            SDL_RenderFillRect(renderer, &inset);

            render_text(
                renderer,
                fonts_,
                candidates_[i].parent_path().filename().string(),
                label_rect,
                TextStyle{22, true, SDL_Color{255, 255, 255, 255}, TextAlign::Left});
        }
    }

    if (!status_message_.empty()) {
        const SDL_Rect status_rect{
            kPadding, canvas_height - 40, canvas_width - 2 * kPadding, 30};
        render_text(
            renderer,
            fonts_,
            status_message_,
            status_rect,
            TextStyle{16, false, SDL_Color{255, 80, 80, 255}, TextAlign::Left});
    }
}

}  // namespace cherry
