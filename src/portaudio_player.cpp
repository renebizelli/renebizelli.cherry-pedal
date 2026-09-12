#include "portaudio_player.hpp"

#include "wav_decoder.hpp"

namespace cherry {

namespace {

// Adapts whatever channel layout the WAV file has to the shared channel's
// fixed layout. Real project WAVs are expected to already match (see PRD),
// so this only needs to cover the common mono-to-stereo case gracefully
// instead of failing outright.
std::vector<float> to_channel_format(const DecodedAudio& decoded) {
    const int target_channels = PortAudioChannel::channel_count();

    if (decoded.channels == target_channels) {
        return decoded.interleaved_samples;
    }

    const std::size_t frames = decoded.interleaved_samples.size() /
        static_cast<std::size_t>(decoded.channels);
    std::vector<float> converted(frames * static_cast<std::size_t>(target_channels), 0.0f);

    for (std::size_t frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < target_channels; ++channel) {
            const int source_channel = channel % decoded.channels;
            converted[frame * target_channels + channel] =
                decoded.interleaved_samples[frame * decoded.channels + source_channel];
        }
    }

    return converted;
}

}  // namespace

PortAudioPlayer::PortAudioPlayer(std::shared_ptr<PortAudioChannel> channel)
    : channel_(std::move(channel)) {}

void PortAudioPlayer::load(const std::string& file) {
    const DecodedAudio decoded = decode_wav_file(file);
    stereo_samples_ = std::make_shared<const std::vector<float>>(to_channel_format(decoded));
}

void PortAudioPlayer::play() {
    channel_->play(stereo_samples_);
}

void PortAudioPlayer::stop() {
    channel_->stop();
}

bool PortAudioPlayer::is_playing() const {
    return channel_->is_playing();
}

}  // namespace cherry
