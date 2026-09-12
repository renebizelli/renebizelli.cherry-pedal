#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "fake_audio_player.hpp"
#include "models.hpp"
#include "playback_sequencer.hpp"
#include "player_service.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::EventRecorder;
using cherry::testing::fail;
using cherry::testing::FakeAudioPlayer;
using cherry::testing::FakeChannel;
using cherry::testing::wait_until;

namespace {

// Reuses a real WAV from the project so PlayerService's existence check in
// its constructor passes without needing a fixture file of our own.
Audio existing_audio_fixture() {
    return Audio{"audio", "audio", "../bands/ask/final/final.wav"};
}

void test_play_then_natural_end_fires_starts_then_ends() {
    EventRecorder events;
    PlaybackSequencer sequencer;
    auto channel = std::make_shared<FakeChannel>();

    PlayerService service(
        existing_audio_fixture(),
        std::make_unique<FakeAudioPlayer>(channel),
        sequencer,
        std::ref(events),
        []() {});

    service.play();

    if (events.snapshot() != std::vector<AudioEvent>{AudioEvent::Starts}) {
        fail("expected AUDIO_STARTS right after play()");
    }

    channel->finish();

    if (!wait_until([&] { return events.snapshot().size() == 2; }, std::chrono::milliseconds(500))) {
        fail("expected AUDIO_ENDS after the clip finishes naturally");
    }

    std::cout << "OK: play() then natural end fires STARTS then ENDS\n";
}

void test_stop_fires_ends_event() {
    EventRecorder events;
    PlaybackSequencer sequencer;
    auto channel = std::make_shared<FakeChannel>();

    PlayerService service(
        existing_audio_fixture(),
        std::make_unique<FakeAudioPlayer>(channel),
        sequencer,
        std::ref(events),
        []() {});

    service.play();
    service.stop();

    const std::vector<AudioEvent> expected{AudioEvent::Starts, AudioEvent::Ends};
    if (events.snapshot() != expected) {
        fail("expected STARTS then ENDS after play() then stop()");
    }

    std::cout << "OK: stop() after play() fires ENDS\n";
}

void test_autoforward_runs_only_on_natural_end() {
    EventRecorder events;
    PlaybackSequencer sequencer;
    auto channel = std::make_shared<FakeChannel>();
    std::atomic<int> autoforward_calls{0};

    PlayerService service(
        existing_audio_fixture(),
        std::make_unique<FakeAudioPlayer>(channel),
        sequencer,
        std::ref(events),
        [&]() { ++autoforward_calls; });

    service.play();
    service.stop();

    if (autoforward_calls.load() != 0) {
        fail("stop() must not trigger autoforward");
    }

    service.play();
    channel->finish();

    if (!wait_until([&] { return autoforward_calls.load() == 1; }, std::chrono::milliseconds(500))) {
        fail("a natural end must trigger autoforward exactly once");
    }

    std::cout << "OK: autoforward runs only on natural end, not on stop()\n";
}

void test_stop_on_superseded_player_does_not_fire_stale_end() {
    EventRecorder events_a;
    EventRecorder events_b;
    PlaybackSequencer sequencer;
    auto channel = std::make_shared<FakeChannel>();

    PlayerService service_a(
        existing_audio_fixture(),
        std::make_unique<FakeAudioPlayer>(channel),
        sequencer,
        std::ref(events_a),
        []() {});

    PlayerService service_b(
        existing_audio_fixture(),
        std::make_unique<FakeAudioPlayer>(channel),
        sequencer,
        std::ref(events_b),
        []() {});

    service_a.play();
    service_b.play();  // takes over the shared channel/sequencer before A ends
    service_a.stop();

    if (events_a.snapshot() != std::vector<AudioEvent>{AudioEvent::Starts}) {
        fail("a player superseded by another must not fire a stale ENDS on stop()");
    }

    std::cout << "OK: stop() on a superseded player does not fire a stale ENDS\n";
}

}  // namespace

int main() {
    test_play_then_natural_end_fires_starts_then_ends();
    test_stop_fires_ends_event();
    test_autoforward_runs_only_on_natural_end();
    test_stop_on_superseded_player_does_not_fire_stale_end();

    std::cout << "All PlayerService checks passed\n";
    return 0;
}
