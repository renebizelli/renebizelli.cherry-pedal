#pragma once

#include <atomic>
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
class PortAudioChannel {
public:
    PortAudioChannel();
    ~PortAudioChannel();

    PortAudioChannel(const PortAudioChannel&) = delete;
    PortAudioChannel& operator=(const PortAudioChannel&) = delete;

    // `stereo_samples` must already be interleaved at channel_count()
    // channels and sample_rate() Hz.
    void play(std::shared_ptr<const std::vector<float>> stereo_samples);
    void stop();
    bool is_playing() const;

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

    int render(float* output, unsigned long frame_count);

    PaStream* stream_ = nullptr;
    std::shared_ptr<const std::vector<float>> buffer_;
    std::atomic<std::size_t> position_{0};
    std::atomic<bool> playing_{false};
};

}  // namespace cherry
