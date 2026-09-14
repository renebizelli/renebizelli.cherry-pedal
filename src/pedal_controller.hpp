#pragma once

#include <vector>

#include "audio_player_factory.hpp"
#include "models.hpp"
#include "playback_sequencer.hpp"
#include "player_service.hpp"
#include "song_service.hpp"

namespace cherry {

// Facade exposing the session-level actions the UI/input layer needs:
// navigate songs/audios and play/stop. Owns the PlaybackSequencer for the
// session, since only one PedalController is ever active at a time (a new
// one replaces it whenever the band is reselected).
class PedalController {
public:
    PedalController(
        std::vector<Song> songs,
        AudioPlayerFactory& player_factory,
        PlayerService::EventCallback callback,
        LoadProgressCallback load_progress = nullptr);

    const Song* current_song() const;
    const Audio* current_audio() const;

    const Song* next_song();
    const Song* previous_song();
    const Song* next_audio();

    void play();
    void stop();
    double current_progress() const;

private:
    PlaybackSequencer sequencer_;
    SongService song_service_;
};

}  // namespace cherry
