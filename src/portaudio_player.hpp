#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "audio_player.hpp"
#include "portaudio_channel.hpp"

namespace cherry {

// AudioPlayer implementation backed by PortAudio: decodes and converts a WAV
// file to the shared channel's format once (on load(), i.e. preload time),
// then hands the ready-to-play buffer to the shared PortAudioChannel on
// play() — no decoding happens on the low-latency trigger path.
class PortAudioPlayer : public AudioPlayer {
public:
    explicit PortAudioPlayer(std::shared_ptr<PortAudioChannel> channel);

    void load(const std::string& file) override;
    void play() override;
    void stop() override;
    bool is_playing() const override;

private:
    std::shared_ptr<PortAudioChannel> channel_;
    std::shared_ptr<const std::vector<std::int16_t>> pcm_samples_;
};

}  // namespace cherry
