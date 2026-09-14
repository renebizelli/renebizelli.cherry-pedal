#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <thread>

#include "audio_events.hpp"
#include "audio_player.hpp"
#include "models.hpp"
#include "playback_sequencer.hpp"

namespace cherry {

// Orchestrates play/stop for one preloaded audio clip: starts a background
// watcher that waits for the shared channel to go idle, then decides — via
// PlaybackSequencer — whether this clip's own playback actually ended (firing
// events and, on a natural end, triggering autoforward) or whether the
// channel was simply taken over by a newer play() elsewhere, in which case it
// stays silent.
class PlayerService {
public:
    using EventCallback = std::function<void(AudioEvent)>;
    using AutoforwardCallback = std::function<void()>;

    PlayerService(
        Audio audio,
        std::unique_ptr<AudioPlayer> player,
        PlaybackSequencer& sequencer,
        EventCallback callback,
        AutoforwardCallback autoforward);

    ~PlayerService();

    PlayerService(const PlayerService&) = delete;
    PlayerService& operator=(const PlayerService&) = delete;

    void play();
    void stop();
    double progress() const;

private:
    enum class EndReason { ByPlay, ByStop };

    void watch_for_end(PlaybackSequencer::PlayId play_id);
    void join_watcher();

    Audio audio_;
    std::unique_ptr<AudioPlayer> player_;
    PlaybackSequencer& sequencer_;
    EventCallback callback_;
    AutoforwardCallback autoforward_;

    std::thread watcher_;
    std::atomic<EndReason> end_reason_{EndReason::ByStop};
    std::optional<PlaybackSequencer::PlayId> last_play_id_;
};

}  // namespace cherry
