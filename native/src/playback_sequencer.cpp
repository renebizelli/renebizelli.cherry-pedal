#include "playback_sequencer.hpp"

namespace cherry {

PlaybackSequencer::PlayId PlaybackSequencer::begin_play() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++sequence_;
    current_owner_ = sequence_;
    return sequence_;
}

bool PlaybackSequencer::owns(PlayId play_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return current_owner_.has_value() && *current_owner_ == play_id;
}

void PlaybackSequencer::release_if_owner(PlayId play_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (current_owner_.has_value() && *current_owner_ == play_id) {
        current_owner_.reset();
    }
}

}  // namespace cherry
