#include "audio_service.hpp"

namespace cherry {

AudioService::AudioService(
    Song song,
    AudioPlayerFactory& player_factory,
    PlaybackSequencer& sequencer,
    PlayerService::EventCallback callback,
    LoadProgressCallback load_progress)
    : song_(std::move(song)) {
    const std::size_t total = song_.audios.size();
    players_.reserve(total);

    for (const auto& audio : song_.audios) {
        players_.push_back(std::make_unique<PlayerService>(
            audio,
            player_factory.create(),
            sequencer,
            callback,
            [this]() { autoforward(); }));

        if (load_progress) {
            load_progress(players_.size(), total);
        }
    }

    std::lock_guard<std::mutex> lock(mutex_);
    select_current_audio_locked();
}

void AudioService::select_current_audio_locked() {
    if (current_player_ != nullptr) {
        current_player_->stop();
    }

    current_player_ = players_[index_].get();
}

void AudioService::advance_locked() {
    ++index_;
    if (index_ == song_.audios.size()) {
        index_ = 0;
    }

    select_current_audio_locked();
}

void AudioService::forward() {
    std::lock_guard<std::mutex> lock(mutex_);
    advance_locked();
}

void AudioService::autoforward() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (song_.autoforward) {
        advance_locked();
    }
}

const Audio& AudioService::current() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return song_.audios[index_];
}

PlayerService* AudioService::current_player() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return current_player_;
}

void AudioService::play() {
    current_player()->play();
}

void AudioService::stop() {
    current_player()->stop();
}

double AudioService::progress() const {
    return current_player()->progress();
}

}  // namespace cherry
