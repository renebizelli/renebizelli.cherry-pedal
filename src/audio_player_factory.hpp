#pragma once

#include <memory>

#include "audio_player.hpp"

namespace cherry {

// Lets AudioService create AudioPlayer instances without knowing which
// backend is in use — production code injects a PortAudioPlayerFactory,
// tests inject a FakeAudioPlayerFactory.
class AudioPlayerFactory {
public:
    virtual ~AudioPlayerFactory() = default;

    virtual std::unique_ptr<AudioPlayer> create() const = 0;
};

}  // namespace cherry
