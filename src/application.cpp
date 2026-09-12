#include "application.hpp"

#include <chrono>
#include <thread>

namespace cherry {

namespace {
// GPIO pin bindings mirror the original app's SetupScreen/PainelScreen
// exactly (see the port plan).
constexpr unsigned int kSetupPinAdvance = 17;   // roxo
constexpr unsigned int kSetupPinConfirm = 22;   // azul

constexpr unsigned int kPanelPinSongBackward = 17;   // roxo
constexpr unsigned int kPanelPinPlay = 22;            // azul
constexpr unsigned int kPanelPinStop = 23;            // amarelo
constexpr unsigned int kPanelPinSongForward = 24;     // orange
constexpr unsigned int kPanelPinAudioForward = 27;    // vermelho

constexpr auto kFrameInterval = std::chrono::milliseconds(33);  // ~30 FPS
}  // namespace

Application::Application(
    SourceService& source,
    AudioPlayerFactory& player_factory,
    GpioButtonFactory& button_factory,
    FontCache& fonts,
    TextureCache& textures,
    std::string assets_dir)
    : source_(source),
      player_factory_(player_factory),
      button_factory_(button_factory),
      fonts_(fonts),
      textures_(textures),
      assets_dir_(std::move(assets_dir)) {}

void Application::show_setup() {
    render_loading_screen();

    input_service_.stop();
    input_service_ = InputService();
    panel_screen_.reset();

    setup_screen_ =
        std::make_unique<SetupScreen>(source_.bands(), *this, fonts_, textures_, assets_dir_);

    bind_setup_inputs();
    active_screen_ = ActiveScreen::Setup;
}

void Application::show_panel(const Band& band) {
    // Copied before setup_screen_ (which owns the Band `band` may reference,
    // if the caller was e.g. SetupScreen::confirm_selection()) is reset.
    const Band selected_band = band;

    render_loading_screen();

    input_service_.stop();
    input_service_ = InputService();
    setup_screen_.reset();

    panel_screen_ = std::make_unique<PainelScreen>(
        selected_band, source_.songs(selected_band), player_factory_, *this, fonts_, textures_);

    bind_panel_inputs();
    active_screen_ = ActiveScreen::Panel;
}

void Application::quit() {
    running_ = false;
}

void Application::bind_setup_inputs() {
    input_service_.add_button(button_factory_.create(kSetupPinAdvance, [this] {
        actions_.push([this] {
            if (setup_screen_) {
                setup_screen_->advance_selection();
            }
        });
    }));
    input_service_.add_button(button_factory_.create(kSetupPinConfirm, [this] {
        actions_.push([this] {
            if (setup_screen_) {
                setup_screen_->confirm_selection();
            }
        });
    }));
    input_service_.start();
}

void Application::bind_panel_inputs() {
    input_service_.add_button(button_factory_.create(kPanelPinSongBackward, [this] {
        actions_.push([this] {
            if (panel_screen_) {
                panel_screen_->song_backward();
            }
        });
    }));
    input_service_.add_button(button_factory_.create(kPanelPinPlay, [this] {
        actions_.push([this] {
            if (panel_screen_) {
                panel_screen_->do_play();
            }
        });
    }));
    input_service_.add_button(button_factory_.create(kPanelPinStop, [this] {
        actions_.push([this] {
            if (panel_screen_) {
                panel_screen_->do_stop();
            }
        });
    }));
    input_service_.add_button(button_factory_.create(kPanelPinSongForward, [this] {
        actions_.push([this] {
            if (panel_screen_) {
                panel_screen_->song_forward();
            }
        });
    }));
    input_service_.add_button(button_factory_.create(kPanelPinAudioForward, [this] {
        actions_.push([this] {
            if (panel_screen_) {
                panel_screen_->audio_forward();
            }
        });
    }));
    input_service_.start();
}

void Application::handle_key(SDL_Keycode key) {
    if (active_screen_ == ActiveScreen::Setup && setup_screen_ != nullptr) {
        setup_screen_->handle_key(key);
    } else if (active_screen_ == ActiveScreen::Panel && panel_screen_ != nullptr) {
        panel_screen_->handle_key(key);
    }
}

void Application::render_loading_screen() {
    if (renderer_ == nullptr) {
        return;
    }

    SplashScreen splash(textures_, assets_dir_ + "/cherry.jpg");
    splash.render(renderer_, canvas_width_, canvas_height_);
    SDL_RenderPresent(renderer_);
}

void Application::run(SDL_Renderer* renderer, int canvas_width, int canvas_height) {
    renderer_ = renderer;
    canvas_width_ = canvas_width;
    canvas_height_ = canvas_height;

    render_loading_screen();
    SDL_Delay(1500);

    show_setup();

    while (running_) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit();
            } else if (event.type == SDL_KEYDOWN) {
                handle_key(event.key.keysym.sym);
            }
        }

        if (!running_) {
            break;
        }

        actions_.drain();

        if (active_screen_ == ActiveScreen::Setup && setup_screen_ != nullptr) {
            setup_screen_->render(renderer_, canvas_width_, canvas_height_);
        } else if (active_screen_ == ActiveScreen::Panel && panel_screen_ != nullptr) {
            panel_screen_->render(renderer_, canvas_width_, canvas_height_);
        }

        SDL_RenderPresent(renderer_);
        std::this_thread::sleep_for(kFrameInterval);
    }

    input_service_.stop();
}

}  // namespace cherry
