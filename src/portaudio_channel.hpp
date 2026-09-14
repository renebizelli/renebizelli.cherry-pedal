#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

#include <portaudio.h>

namespace cherry {

// The single physical playback channel every PortAudioPlayer plays through,
// mirroring the pygame Channel(0) single-channel design it replaces: play()
// on any instance immediately takes over whatever was playing.
//
// Owns one persistent PortAudio stream instead of opening/closing a stream
// per play(), which would add latency on every trigger. play()/stop() are
// called from the UI/input thread while render() runs on PortAudio's own
// real-time audio thread, so the shared buffer is exchanged via
// std::atomic_store/std::atomic_load rather than a plain assignment.
//
// Uses 16-bit signed PCM rather than float32: it's the format the Pi's own
// audio codec speaks natively (confirmed via `speaker-test`), avoiding a
// float-to-int conversion in the ALSA "plug" layer that produced silence
// on the real hardware in testing even though the stream opened and the
// decoded audio data itself was correct.
class PortAudioChannel {
public:
    PortAudioChannel();
    ~PortAudioChannel();

    PortAudioChannel(const PortAudioChannel&) = delete;
    PortAudioChannel& operator=(const PortAudioChannel&) = delete;

    // `pcm_samples` must already be interleaved at channel_count() channels
    // and sample_rate() Hz.
    void play(std::shared_ptr<const std::vector<std::int16_t>> pcm_samples);
    void stop();
    bool is_playing() const;

    // Fraction of the current buffer already played, in [0.0, 1.0].
    double progress() const;

    static constexpr double sample_rate() { return 44100.0; }
    static constexpr int channel_count() { return 2; }

private:
    static int render_callback(
        const void* input,
        void* output,
        unsigned long frame_count,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void* user_data);

    int render(std::int16_t* output, unsigned long frame_count);

    PaStream* stream_ = nullptr;
    std::shared_ptr<const std::vector<std::int16_t>> buffer_;
    std::atomic<std::size_t> position_{0};
    std::atomic<bool> playing_{false};
};

}  // namespace cherry
