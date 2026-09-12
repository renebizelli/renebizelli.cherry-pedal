#pragma once

#include <memory>

#include "audio_player_factory.hpp"
#include "portaudio_channel.hpp"
#include "portaudio_player.hpp"

namespace cherry {

// Production AudioPlayerFactory: every created player shares the same
// PortAudioChannel instance, so they all compete for the one physical
// output the same way pygame's Channel(0) did.
class PortAudioPlayerFactory : public AudioPlayerFactory {
public:
    explicit PortAudioPlayerFactory(std::shared_ptr<PortAudioChannel> channel)
        : channel_(std::move(channel)) {}

    std::unique_ptr<AudioPlayer> create() const override {
        return std::make_unique<PortAudioPlayer>(channel_);
    }

private:
    std::shared_ptr<PortAudioChannel> channel_;
};

}  // namespace cherry
