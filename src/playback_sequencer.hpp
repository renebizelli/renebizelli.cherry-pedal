#pragma once

#include <cstdint>
#include <mutex>
#include <optional>

namespace cherry {

// Tracks which play() call currently owns the shared playback channel, so a
// PlayerService's end-of-playback watcher can tell whether the audio it was
// watching finished naturally or was pre-empted by a newer play() before
// firing events or triggering autoforward. Shared, thread-safe collaborator
// injected into every PlayerService of a song, replacing what the original
// implementation tracked with process-wide static state.
class PlaybackSequencer {
public:
    using PlayId = std::uint64_t;

    PlayId begin_play();
    bool owns(PlayId play_id) const;
    void release_if_owner(PlayId play_id);

private:
    mutable std::mutex mutex_;
    PlayId sequence_ = 0;
    std::optional<PlayId> current_owner_;
};

}  // namespace cherry
