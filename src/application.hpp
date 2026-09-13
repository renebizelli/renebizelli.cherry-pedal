#pragma once

#include <memory>
#include <string>

#include <SDL.h>

#include "action_queue.hpp"
#include "audio_player_factory.hpp"
#include "font_cache.hpp"
#include "gpio_button_factory.hpp"
#include "input_service.hpp"
#include "models.hpp"
#include "navigator.hpp"
#include "screens/painel_screen.hpp"
#include "screens/setup_screen.hpp"
#include "screens/splash_screen.hpp"
#include "screens/sync_screen.hpp"
#include "source_service.hpp"
#include "texture_cache.hpp"

namespace cherry {

// Composition root: owns the current screen and its GPIO bindings, runs
// the main loop, and dispatches keyboard/GPIO input to whichever screen is
// active. Implements Navigator so screens can request a screen switch or
// quit without knowing about this class or each other.
class Application : public Navigator {
public:
    Application(
        SourceService& source,
        AudioPlayerFactory& player_factory,
        GpioButtonFactory& button_factory,
        FontCache& fonts,
        TextureCache& textures,
        std::string assets_dir);

    void show_setup() override;
    void show_panel(const Band& band) override;
    void show_sync(const Band& band) override;
    void quit() override;
    void request_restart() override;

    void run(SDL_Renderer* renderer, int canvas_width, int canvas_height);
    bool restart_requested() const { return restart_requested_; }

private:
    enum class ActiveScreen { Splash, Setup, Panel, Sync };

    void bind_setup_inputs();
    void bind_panel_inputs();
    void handle_key(SDL_Keycode key);
    void handle_click(int window_x, int window_y);
    void render_loading_screen();

    SourceService& source_;
    AudioPlayerFactory& player_factory_;
    GpioButtonFactory& button_factory_;
    FontCache& fonts_;
    TextureCache& textures_;
    std::string assets_dir_;

    SDL_Renderer* renderer_ = nullptr;
    int canvas_width_ = 0;
    int canvas_height_ = 0;

    ActionQueue actions_;
    ActiveScreen active_screen_ = ActiveScreen::Splash;
    std::unique_ptr<SetupScreen> setup_screen_;
    std::unique_ptr<PainelScreen> panel_screen_;
    std::unique_ptr<SyncScreen> sync_screen_;
    InputService input_service_;
    bool running_ = true;
    bool restart_requested_ = false;
};

}  // namespace cherry
