#include "portaudio_channel.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace cherry {

namespace {
constexpr unsigned long kFramesPerBuffer = 256;
}

PortAudioChannel::PortAudioChannel() {
    PaError error = Pa_Initialize();
    if (error != paNoError) {
        throw std::runtime_error(std::string("PortAudio init failed: ") + Pa_GetErrorText(error));
    }

    error = Pa_OpenDefaultStream(
        &stream_,
        0,
        channel_count(),
        paInt16,
        sample_rate(),
        kFramesPerBuffer,
        &PortAudioChannel::render_callback,
        this);

    if (error != paNoError) {
        Pa_Terminate();
        throw std::runtime_error(std::string("PortAudio open stream failed: ") + Pa_GetErrorText(error));
    }

    const PaDeviceIndex device_index = Pa_GetDefaultOutputDevice();
    const PaDeviceInfo* device_info = Pa_GetDeviceInfo(device_index);
    std::cerr << "PortAudio output device: "
              << (device_info != nullptr ? device_info->name : "<unknown>") << " (index "
              << device_index << ")\n";

    error = Pa_StartStream(stream_);
    if (error != paNoError) {
        Pa_CloseStream(stream_);
        Pa_Terminate();
        throw std::runtime_error(std::string("PortAudio start stream failed: ") + Pa_GetErrorText(error));
    }
}

PortAudioChannel::~PortAudioChannel() {
    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
    Pa_Terminate();
}

void PortAudioChannel::play(std::shared_ptr<const std::vector<std::int16_t>> pcm_samples) {
    position_.store(0, std::memory_order_relaxed);
    std::atomic_store(&buffer_, std::move(pcm_samples));
    playing_.store(true, std::memory_order_release);
}

void PortAudioChannel::stop() {
    playing_.store(false, std::memory_order_release);
}

bool PortAudioChannel::is_playing() const {
    return playing_.load(std::memory_order_acquire);
}

int PortAudioChannel::render_callback(
    const void* /*input*/,
    void* output,
    unsigned long frame_count,
    const PaStreamCallbackTimeInfo* /*time_info*/,
    PaStreamCallbackFlags /*status_flags*/,
    void* user_data) {
    return static_cast<PortAudioChannel*>(user_data)->render(static_cast<std::int16_t*>(output), frame_count);
}

int PortAudioChannel::render(std::int16_t* output, unsigned long frame_count) {
    const std::size_t frame_samples = static_cast<std::size_t>(frame_count) * channel_count();
    const auto buffer = std::atomic_load(&buffer_);

    if (!playing_.load(std::memory_order_acquire) || buffer == nullptr) {
        std::fill_n(output, frame_samples, static_cast<std::int16_t>(0));
        return paContinue;
    }

    std::size_t position = position_.load(std::memory_order_relaxed);

    for (std::size_t i = 0; i < frame_samples; ++i) {
        output[i] = (position < buffer->size()) ? (*buffer)[position++] : static_cast<std::int16_t>(0);
    }

    position_.store(position, std::memory_order_relaxed);

    if (position >= buffer->size()) {
        playing_.store(false, std::memory_order_release);
    }

    return paContinue;
}

}  // namespace cherry
