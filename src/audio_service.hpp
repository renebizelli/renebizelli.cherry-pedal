#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>

#include "audio_player_factory.hpp"
#include "models.hpp"
#include "playback_sequencer.hpp"
#include "player_service.hpp"

namespace cherry {

// Preloads one PlayerService per audio of a song and keeps track of which
// one is "current", stopping the previous clip whenever the selection moves.
//
// index_/current_player_ can be mutated both by the UI/input thread (via
// forward()) and by a PlayerService's background watcher thread (via the
// autoforward callback firing when a clip ends naturally) — unlike the
// original Python, which leaned on the GIL for this, access here is guarded
// by an explicit mutex.
class AudioService {
public:
    AudioService(
        Song song,
        AudioPlayerFactory& player_factory,
        PlaybackSequencer& sequencer,
        PlayerService::EventCallback callback);

    void forward();
    const Audio& current() const;
    void play();
    void stop();

private:
    void advance_locked();
    void select_current_audio_locked();
    void autoforward();
    PlayerService* current_player() const;

    Song song_;
    mutable std::mutex mutex_;
    std::size_t index_ = 0;
    std::vector<std::unique_ptr<PlayerService>> players_;
    PlayerService* current_player_ = nullptr;
};

}  // namespace cherry
