#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "audio_player_factory.hpp"
#include "audio_service.hpp"
#include "models.hpp"
#include "playback_sequencer.hpp"
#include "player_service.hpp"

namespace cherry {

// Navigates between songs of a band, re-creating the AudioService (and its
// preloaded PlayerServices) whenever the current song changes.
class SongService {
public:
    SongService(
        AudioPlayerFactory& player_factory,
        PlaybackSequencer& sequencer,
        PlayerService::EventCallback callback,
        LoadProgressCallback load_progress = nullptr);

    void set_songs(std::vector<Song> songs);

    void forward();
    void backward();
    const Song* current() const;

    void next_audio();
    const Audio* current_audio() const;

    void play();
    void stop();
    double current_progress() const;

private:
    // progress is only ever supplied for the very first song of a freshly
    // selected band (see set_songs()); forward()/backward() reload silently
    // — same as before this callback existed — since those happen while
    // already inside the panel, not during the band-selection transition
    // this is meant to give feedback for.
    void init_audio(LoadProgressCallback progress = nullptr);

    AudioPlayerFactory& player_factory_;
    PlaybackSequencer& sequencer_;
    PlayerService::EventCallback callback_;
    LoadProgressCallback load_progress_;

    std::vector<Song> songs_;
    std::size_t index_ = 0;
    std::unique_ptr<AudioService> audio_service_;
};

}  // namespace cherry
