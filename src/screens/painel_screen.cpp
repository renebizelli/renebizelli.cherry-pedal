#include "painel_screen.hpp"

#include <chrono>
#include <cmath>
#include <string>

#include "layout_helpers.hpp"
#include "text_renderer.hpp"

namespace cherry {

namespace {
constexpr int kBandColumnWidth = 560;
constexpr int kIndicatorColumnWidth = 240;
constexpr int kHeaderHeight = 54;
constexpr int kSongHeight = 115;
constexpr int kIndicatorRadius = 70;
constexpr int kHeaderPadding = 16;
constexpr int kSyncButtonHeight = 40;
constexpr auto kIndicatorHideAfterMs = 5000;
constexpr auto kIndicatorBlinkIntervalMs = 500;

std::int64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}
}  // namespace

PainelScreen::PainelScreen(
    Band band,
    std::vector<Song> songs,
    AudioPlayerFactory& player_factory,
    Navigator& navigator,
    FontCache& fonts,
    TextureCache& textures)
    : band_(std::move(band)),
      navigator_(navigator),
      fonts_(fonts),
      textures_(textures),
      controller_(
          std::move(songs), player_factory, [this](AudioEvent event) { on_audio_event(event); }) {}

void PainelScreen::on_audio_event(AudioEvent event) {
    if (event == AudioEvent::Starts) {
        playing_.store(true, std::memory_order_release);
    } else if (event == AudioEvent::Ends) {
        playing_.store(false, std::memory_order_release);
        has_stopped_once_.store(true, std::memory_order_release);
        last_stop_at_ms_.store(now_ms(), std::memory_order_release);
    }
    // AudioEvent::NotFound has nothing further to draw: the missing file
    // was already reported during startup validation.
}

void PainelScreen::song_forward() {
    controller_.next_song();
}

void PainelScreen::song_backward() {
    controller_.previous_song();
}

void PainelScreen::audio_forward() {
    controller_.next_audio();
}

void PainelScreen::do_play() {
    controller_.play();
}

void PainelScreen::do_stop() {
    controller_.stop();
    playing_.store(false, std::memory_order_release);
    has_stopped_once_.store(true, std::memory_order_release);
    last_stop_at_ms_.store(now_ms(), std::memory_order_release);
}

void PainelScreen::back_to_setup() {
    navigator_.show_setup();
}

void PainelScreen::open_sync() {
    navigator_.show_sync(band_);
}

void PainelScreen::handle_key(SDL_Keycode key) {
    switch (key) {
        case SDLK_DOWN:
            audio_forward();
            break;
        case SDLK_LEFT:
            song_backward();
            break;
        case SDLK_RIGHT:
            song_forward();
            break;
        case SDLK_SPACE:
            do_play();
            break;
        case SDLK_ESCAPE:
            do_stop();
            break;
        case SDLK_F1:
            back_to_setup();
            break;
        default:
            break;
    }
}

void PainelScreen::handle_click(int x, int y, int canvas_width, int canvas_height) {
    (void)canvas_width;

    const SDL_Rect sync_button{
        kBandColumnWidth, canvas_height - kSyncButtonHeight, kIndicatorColumnWidth, kSyncButtonHeight};
    const SDL_Point point{x, y};
    if (SDL_PointInRect(&point, &sync_button)) {
        open_sync();
        return;
    }

    // The header (band name) is the "return to setup" target, but on the
    // real resistive touchscreen this was tested on, touches near the
    // physical top edge are measurably less accurate than elsewhere on
    // screen — sometimes reporting a position above the logical canvas
    // entirely (negative y). A generous vertical band, not tied to the
    // header's visual bounds, absorbs that instead of requiring
    // pixel-perfect accuracy right at the edge. Nothing else in the panel
    // is clickable, so accepting the full width here is harmless.
    constexpr int kClickableTop = -80;
    constexpr int kClickableBottom = 150;

    if (y >= kClickableTop && y <= kClickableBottom) {
        back_to_setup();
    }
}

PainelScreen::IndicatorPhase PainelScreen::indicator_phase() const {
    if (playing_.load(std::memory_order_acquire)) {
        return IndicatorPhase::Playing;
    }

    if (!has_stopped_once_.load(std::memory_order_acquire)) {
        return IndicatorPhase::Hidden;
    }

    const auto elapsed = now_ms() - last_stop_at_ms_.load(std::memory_order_acquire);
    return elapsed < kIndicatorHideAfterMs ? IndicatorPhase::Idle : IndicatorPhase::Hidden;
}

void PainelScreen::render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const SDL_Rect band_rect{
        kHeaderPadding, 0, kBandColumnWidth - kHeaderPadding, kHeaderHeight};
    render_text(
        renderer,
        fonts_,
        band_.name,
        band_rect,
        TextStyle{28, true, SDL_Color{255, 255, 255, 255}, TextAlign::Left});

    const Song* song = controller_.current_song();

    const SDL_Rect mode_rect{
        kBandColumnWidth, 0, kIndicatorColumnWidth - kHeaderPadding, kHeaderHeight};
    render_text(
        renderer,
        fonts_,
        (song != nullptr && song->autoforward) ? "AUTO" : "MANUAL",
        mode_rect,
        TextStyle{24, true, SDL_Color{0, 255, 0, 255}, TextAlign::Right});

    const SDL_Rect song_rect{0, kHeaderHeight, canvas_width, kSongHeight};
    if (song != nullptr) {
        render_text(
            renderer,
            fonts_,
            song->name,
            song_rect,
            TextStyle{song_font_size(song->name), true, SDL_Color{255, 255, 0, 255}});
    }

    const int list_top = kHeaderHeight + kSongHeight;
    const int list_height = canvas_height - list_top;

    if (song != nullptr && !song->audios.empty()) {
        const Audio* current_audio = controller_.current_audio();
        const int row_height = audio_row_height(song->audios.size());
        const int font_size = audio_font_size(song->audios.size());

        for (std::size_t i = 0; i < song->audios.size(); ++i) {
            const Audio& audio = song->audios[i];
            const bool selected = current_audio != nullptr && current_audio->id == audio.id;
            const SDL_Rect row{
                0, list_top + static_cast<int>(i) * row_height, kBandColumnWidth, row_height};

            if (selected) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &row);
            }

            const std::string label =
                (selected ? "> " : "") + std::to_string(i + 1) + " " + audio.name;
            const SDL_Rect label_rect{
                row.x + kHeaderPadding, row.y, row.w - kHeaderPadding, row.h};
            render_text(
                renderer,
                fonts_,
                label,
                label_rect,
                TextStyle{
                    font_size,
                    true,
                    selected ? SDL_Color{255, 255, 255, 255} : SDL_Color{255, 255, 0, 255},
                    TextAlign::Left});
        }
    }

    const IndicatorPhase phase = indicator_phase();
    if (phase != IndicatorPhase::Hidden) {
        const int center_x = kBandColumnWidth + kIndicatorColumnWidth / 2;
        const int center_y = list_top + list_height / 2;

        SDL_Color color{139, 0, 0, 255};  // darkred (stopped/idle)
        if (phase == IndicatorPhase::Playing) {
            const bool blink_on = (now_ms() / kIndicatorBlinkIntervalMs) % 2 == 0;
            color = blink_on ? SDL_Color{0, 128, 0, 255} : SDL_Color{0, 90, 0, 255};
        }

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        for (int dy = -kIndicatorRadius; dy <= kIndicatorRadius; ++dy) {
            const int half_width = static_cast<int>(
                std::sqrt(static_cast<double>(kIndicatorRadius * kIndicatorRadius - dy * dy)));
            SDL_RenderDrawLine(
                renderer, center_x - half_width, center_y + dy, center_x + half_width, center_y + dy);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        if (phase == IndicatorPhase::Playing) {
            constexpr int kTriangleHalfHeight = 28;
            constexpr int kTriangleWidth = 32;
            const SDL_Vertex triangle[3] = {
                {{static_cast<float>(center_x - kTriangleWidth / 3), static_cast<float>(center_y - kTriangleHalfHeight)}, {255, 255, 255, 255}, {0, 0}},
                {{static_cast<float>(center_x - kTriangleWidth / 3), static_cast<float>(center_y + kTriangleHalfHeight)}, {255, 255, 255, 255}, {0, 0}},
                {{static_cast<float>(center_x + 2 * kTriangleWidth / 3), static_cast<float>(center_y)}, {255, 255, 255, 255}, {0, 0}},
            };
            SDL_RenderGeometry(renderer, nullptr, triangle, 3, nullptr, 0);
        } else {
            constexpr int kSquareSize = 44;
            const SDL_Rect square{
                center_x - kSquareSize / 2, center_y - kSquareSize / 2, kSquareSize, kSquareSize};
            SDL_RenderFillRect(renderer, &square);
        }
    }

    const SDL_Rect sync_button{
        kBandColumnWidth, canvas_height - kSyncButtonHeight, kIndicatorColumnWidth, kSyncButtonHeight};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &sync_button);
    render_text(
        renderer, fonts_, "Sincronizar", sync_button, TextStyle{16, true, SDL_Color{200, 200, 200, 255}});
}

}  // namespace cherry
