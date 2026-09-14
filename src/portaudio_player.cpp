#include "portaudio_player.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "resampler.hpp"
#include "wav_decoder.hpp"

namespace cherry {

namespace {

// Adapts whatever channel layout the WAV file has to the shared channel's
// fixed layout (duplicating mono to both channels; real project WAVs are
// expected to already match — see PRD) and converts already-resampled,
// normalized float samples to the 16-bit PCM the channel plays.
std::vector<std::int16_t> to_channel_format(const std::vector<float>& samples, int source_channels) {
    const int target_channels = PortAudioChannel::channel_count();
    const std::size_t frames = samples.size() / static_cast<std::size_t>(source_channels);

    std::vector<std::int16_t> converted(frames * static_cast<std::size_t>(target_channels), 0);

    for (std::size_t frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < target_channels; ++channel) {
            const int source_channel = channel % source_channels;
            const float sample = samples[frame * source_channels + source_channel];
            const float clamped = std::clamp(sample, -1.0f, 1.0f);
            converted[frame * target_channels + channel] = static_cast<std::int16_t>(clamped * 32767.0f);
        }
    }

    return converted;
}

}  // namespace

PortAudioPlayer::PortAudioPlayer(std::shared_ptr<PortAudioChannel> channel)
    : channel_(std::move(channel)) {}

void PortAudioPlayer::load(const std::string& file) {
    const DecodedAudio decoded = decode_wav_file(file);
    const std::vector<float> resampled = resample_linear(
        decoded.interleaved_samples, decoded.channels, decoded.sample_rate, PortAudioChannel::sample_rate());
    auto converted = std::make_shared<std::vector<std::int16_t>>(to_channel_format(resampled, decoded.channels));

    std::int16_t peak = 0;
    for (std::int16_t sample : *converted) {
        peak = std::max(peak, static_cast<std::int16_t>(std::abs(sample)));
    }

    std::cerr << "Loaded " << file << ": " << decoded.channels << "ch @ " << decoded.sample_rate
              << "Hz (resampled to " << PortAudioChannel::sample_rate() << "Hz), " << converted->size()
              << " samples, peak amplitude " << peak << "\n";

    pcm_samples_ = std::move(converted);
}

void PortAudioPlayer::play() {
    channel_->play(pcm_samples_);
}

void PortAudioPlayer::stop() {
    channel_->stop();
}

bool PortAudioPlayer::is_playing() const {
    return channel_->is_playing();
}

double PortAudioPlayer::progress() const {
    return channel_->progress();
}

}  // namespace cherry
