#pragma once

#include <atomic>
#include <memory>
#include <string>

#include "audio_player.hpp"
#include "audio_player_factory.hpp"

namespace cherry::testing {

// Simulates the single shared hardware channel every AudioPlayer instance
// created by the same factory plays through: play() on any instance stops
// whatever was playing before, mirroring the real single-channel backend.
// Tests decide when playback "naturally" ends by calling finish().
class FakeChannel {
public:
    void play() { playing_.store(true); }
    void stop() { playing_.store(false); }
    bool is_playing() const { return playing_.load(); }
    void finish() { playing_.store(false); }

    double progress() const { return progress_.load(); }
    void set_progress(double value) { progress_.store(value); }

private:
    std::atomic<bool> playing_{false};
    std::atomic<double> progress_{0.0};
};

class FakeAudioPlayer : public AudioPlayer {
public:
    explicit FakeAudioPlayer(std::shared_ptr<FakeChannel> channel) : channel_(std::move(channel)) {}

    void load(const std::string& file) override { loaded_file_ = file; }
    void play() override { channel_->play(); }
    void stop() override { channel_->stop(); }
    bool is_playing() const override { return channel_->is_playing(); }
    double progress() const override { return channel_->progress(); }

    const std::string& loaded_file() const { return loaded_file_; }

private:
    std::shared_ptr<FakeChannel> channel_;
    std::string loaded_file_;
};

class FakeAudioPlayerFactory : public AudioPlayerFactory {
public:
    explicit FakeAudioPlayerFactory(std::shared_ptr<FakeChannel> channel) : channel_(std::move(channel)) {}

    std::unique_ptr<AudioPlayer> create() const override {
        return std::make_unique<FakeAudioPlayer>(channel_);
    }

private:
    std::shared_ptr<FakeChannel> channel_;
};

}  // namespace cherry::testing
