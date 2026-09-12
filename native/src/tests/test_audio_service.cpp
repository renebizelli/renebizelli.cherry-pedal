#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "audio_service.hpp"
#include "fake_audio_player.hpp"
#include "models.hpp"
#include "pedal_controller.hpp"
#include "playback_sequencer.hpp"
#include "song_service.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::EventRecorder;
using cherry::testing::fail;
using cherry::testing::FakeAudioPlayerFactory;
using cherry::testing::FakeChannel;
using cherry::testing::wait_until;

namespace {

// Reuses real WAVs from the project so PlayerService's existence check
// passes without needing fixture files of our own.
Song two_audio_song(bool autoforward) {
    return Song{
        "song-1",
        "Song 1",
        autoforward,
        {
            Audio{"a1", "Audio 1", "../bands/ask/final/final.wav"},
            Audio{"a2", "Audio 2", "../bands/ask/rubber-ring/final.wav"},
        }};
}

void test_audio_service_forward_wraps_and_stops_previous() {
    EventRecorder events;
    PlaybackSequencer sequencer;
    auto channel = std::make_shared<FakeChannel>();
    FakeAudioPlayerFactory factory(channel);

    AudioService service(two_audio_song(false), factory, sequencer, std::ref(events));

    if (service.current().id != "a1") {
        fail("expected the first audio to be current initially");
    }

    service.play();
    if (!channel->is_playing()) {
        fail("expected the channel to be playing after play()");
    }

    service.forward();
    if (service.current().id != "a2") {
        fail("expected forward() to select the second audio");
    }
    if (channel->is_playing()) {
        fail("expected forward() to stop the previous audio");
    }

    service.forward();
    if (service.current().id != "a1") {
        fail("expected forward() to wrap back to the first audio");
    }

    std::cout << "OK: AudioService.forward() wraps and stops the previous audio\n";
}

void test_audio_service_autoforward_moves_selection_without_playing() {
    EventRecorder events;
    PlaybackSequencer sequencer;
    auto channel = std::make_shared<FakeChannel>();
    FakeAudioPlayerFactory factory(channel);

    AudioService service(two_audio_song(true), factory, sequencer, std::ref(events));

    service.play();
    channel->finish();

    if (!wait_until([&] { return service.current().id == "a2"; }, std::chrono::milliseconds(500))) {
        fail("expected autoforward to select the next audio after a natural end");
    }

    if (channel->is_playing()) {
        fail("autoforward must move the selection without auto-playing the next audio");
    }

    std::cout << "OK: AUTO mode advances the selection without auto-playing it\n";
}

void test_song_service_navigation_wraps() {
    EventRecorder events;
    auto channel = std::make_shared<FakeChannel>();
    FakeAudioPlayerFactory factory(channel);
    PlaybackSequencer sequencer;

    Song song_1 = two_audio_song(false);
    Song song_2 = two_audio_song(false);
    song_2.id = "song-2";

    SongService service(factory, sequencer, std::ref(events));
    service.set_songs({song_1, song_2});

    if (service.current()->id != "song-1") {
        fail("expected the first song to be current initially");
    }

    service.forward();
    if (service.current()->id != "song-2") {
        fail("expected forward() to select the second song");
    }

    service.forward();
    if (service.current()->id != "song-1") {
        fail("expected forward() to wrap back to the first song");
    }

    service.backward();
    if (service.current()->id != "song-2") {
        fail("expected backward() to wrap back to the last song");
    }

    std::cout << "OK: SongService navigation wraps in both directions\n";
}

void test_pedal_controller_facade() {
    EventRecorder events;
    auto channel = std::make_shared<FakeChannel>();
    FakeAudioPlayerFactory factory(channel);

    PedalController controller(
        {two_audio_song(false)}, factory, std::ref(events));

    if (controller.current_song() == nullptr || controller.current_song()->id != "song-1") {
        fail("expected the first song to be current initially");
    }

    if (controller.current_audio() == nullptr || controller.current_audio()->id != "a1") {
        fail("expected the first audio to be current initially");
    }

    controller.play();
    if (!channel->is_playing()) {
        fail("expected play() to start the current audio");
    }

    controller.stop();
    if (channel->is_playing()) {
        fail("expected stop() to stop the current audio");
    }

    controller.next_audio();
    if (controller.current_audio()->id != "a2") {
        fail("expected next_audio() to advance within the current song");
    }

    std::cout << "OK: PedalController exposes navigation and play/stop\n";
}

}  // namespace

int main() {
    test_audio_service_forward_wraps_and_stops_previous();
    test_audio_service_autoforward_moves_selection_without_playing();
    test_song_service_navigation_wraps();
    test_pedal_controller_facade();

    std::cout << "All AudioService/SongService/PedalController checks passed\n";
    return 0;
}
