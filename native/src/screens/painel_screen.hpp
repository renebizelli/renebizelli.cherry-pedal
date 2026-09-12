#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

#include <SDL.h>

#include "audio_events.hpp"
#include "audio_player_factory.hpp"
#include "font_cache.hpp"
#include "models.hpp"
#include "navigator.hpp"
#include "pedal_controller.hpp"
#include "texture_cache.hpp"

namespace cherry {

// Operating panel: current band/song/audio, AUTO/MANUAL mode, and the play
// indicator.
//
// Unlike the original Tkinter version, this redraws its full state fresh
// every frame straight from PedalController — so there is no need to port
// the debounce timers or explicit "something changed, redraw" calls the
// original used to batch Tkinter widget updates. The play indicator's
// blink/hide-after-5s timing is likewise derived from elapsed time on each
// render() call rather than scheduled callbacks.
class PainelScreen {
public:
    PainelScreen(
        Band band,
        std::vector<Song> songs,
        AudioPlayerFactory& player_factory,
        Navigator& navigator,
        FontCache& fonts,
        TextureCache& textures);

    void song_forward();
    void song_backward();
    void audio_forward();
    void do_play();
    void do_stop();
    void back_to_setup();

    void handle_key(SDL_Keycode key);
    void render(SDL_Renderer* renderer, int canvas_width, int canvas_height) const;

private:
    enum class IndicatorPhase { Hidden, Idle, Playing };

    // Called from PlayerService's background watcher thread, hence the
    // atomics below instead of plain fields.
    void on_audio_event(AudioEvent event);
    IndicatorPhase indicator_phase() const;

    Band band_;
    Navigator& navigator_;
    FontCache& fonts_;
    TextureCache& textures_;
    PedalController controller_;

    std::atomic<bool> playing_{false};
    std::atomic<bool> has_stopped_once_{false};
    std::atomic<std::int64_t> last_stop_at_ms_{0};
};

}  // namespace cherry
