#include "song_service.hpp"

namespace cherry {

SongService::SongService(
    AudioPlayerFactory& player_factory,
    PlaybackSequencer& sequencer,
    PlayerService::EventCallback callback,
    LoadProgressCallback load_progress)
    : player_factory_(player_factory),
      sequencer_(sequencer),
      callback_(std::move(callback)),
      load_progress_(std::move(load_progress)) {}

void SongService::set_songs(std::vector<Song> songs) {
    songs_ = std::move(songs);
    index_ = 0;
    audio_service_.reset();

    if (!songs_.empty()) {
        init_audio(load_progress_);
    }
}

void SongService::init_audio(LoadProgressCallback progress) {
    if (audio_service_ != nullptr) {
        audio_service_->stop();
    }

    audio_service_ = std::make_unique<AudioService>(
        songs_[index_], player_factory_, sequencer_, callback_, std::move(progress));
}

void SongService::forward() {
    if (songs_.empty()) {
        return;
    }

    ++index_;
    if (index_ == songs_.size()) {
        index_ = 0;
    }

    init_audio();
}

void SongService::backward() {
    if (songs_.empty()) {
        return;
    }

    index_ = (index_ == 0) ? songs_.size() - 1 : index_ - 1;
    init_audio();
}

const Song* SongService::current() const {
    if (songs_.empty()) {
        return nullptr;
    }

    return &songs_[index_];
}

void SongService::next_audio() {
    if (audio_service_ != nullptr) {
        audio_service_->forward();
    }
}

const Audio* SongService::current_audio() const {
    if (audio_service_ == nullptr) {
        return nullptr;
    }

    return &audio_service_->current();
}

void SongService::play() {
    if (audio_service_ != nullptr) {
        audio_service_->play();
    }
}

void SongService::stop() {
    if (audio_service_ != nullptr) {
        audio_service_->stop();
    }
}

double SongService::current_progress() const {
    return audio_service_ != nullptr ? audio_service_->progress() : 0.0;
}

}  // namespace cherry
