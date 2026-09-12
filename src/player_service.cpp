#include "player_service.hpp"

#include <chrono>
#include <filesystem>

#include "errors.hpp"

namespace cherry {

namespace {
constexpr auto kEndPollInterval = std::chrono::milliseconds(10);
}

PlayerService::PlayerService(
    Audio audio,
    std::unique_ptr<AudioPlayer> player,
    PlaybackSequencer& sequencer,
    EventCallback callback,
    AutoforwardCallback autoforward)
    : audio_(std::move(audio)),
      player_(std::move(player)),
      sequencer_(sequencer),
      callback_(std::move(callback)),
      autoforward_(std::move(autoforward)) {
    if (!std::filesystem::is_regular_file(audio_.file)) {
        callback_(AudioEvent::NotFound);
        throw AudioNotFoundError("Audio resource " + audio_.file + " not found");
    }

    player_->load(audio_.file);
}

PlayerService::~PlayerService() {
    join_watcher();
}

void PlayerService::join_watcher() {
    if (watcher_.joinable()) {
        watcher_.join();
    }
}

void PlayerService::play() {
    if (player_->is_playing()) {
        stop();
    }

    join_watcher();

    const auto play_id = sequencer_.begin_play();
    last_play_id_ = play_id;
    end_reason_ = EndReason::ByPlay;

    player_->play();
    watcher_ = std::thread(&PlayerService::watch_for_end, this, play_id);

    callback_(AudioEvent::Starts);
}

void PlayerService::stop() {
    const bool owns_current_play = last_play_id_.has_value() && sequencer_.owns(*last_play_id_);

    end_reason_ = EndReason::ByStop;
    player_->stop();

    if (owns_current_play) {
        sequencer_.release_if_owner(*last_play_id_);
        callback_(AudioEvent::Ends);
    }
}

void PlayerService::watch_for_end(PlaybackSequencer::PlayId play_id) {
    while (player_->is_playing()) {
        std::this_thread::sleep_for(kEndPollInterval);
    }

    if (!sequencer_.owns(play_id)) {
        return;
    }

    sequencer_.release_if_owner(play_id);

    if (end_reason_ == EndReason::ByPlay) {
        autoforward_();
        callback_(AudioEvent::Ends);
    } else {
        callback_(AudioEvent::Ends);
    }
}

}  // namespace cherry
