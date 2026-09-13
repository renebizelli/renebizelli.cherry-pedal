#include "pedal_controller.hpp"

namespace cherry {

PedalController::PedalController(
    std::vector<Song> songs,
    AudioPlayerFactory& player_factory,
    PlayerService::EventCallback callback)
    : song_service_(player_factory, sequencer_, std::move(callback)) {
    song_service_.set_songs(std::move(songs));
}

const Song* PedalController::current_song() const {
    return song_service_.current();
}

const Audio* PedalController::current_audio() const {
    return song_service_.current_audio();
}

const Song* PedalController::next_song() {
    song_service_.forward();
    return current_song();
}

const Song* PedalController::previous_song() {
    song_service_.backward();
    return current_song();
}

const Song* PedalController::next_audio() {
    song_service_.next_audio();
    return current_song();
}

void PedalController::play() {
    song_service_.play();
}

void PedalController::stop() {
    song_service_.stop();
}

}  // namespace cherry
