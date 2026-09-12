#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "debounced_button.hpp"
#include "fake_gpio_event_source.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::fail;
using cherry::testing::FakeGpioEventSource;
using cherry::testing::wait_until;

namespace {

void test_single_press_triggers_once() {
    auto source = std::make_unique<FakeGpioEventSource>();
    auto* raw_source = source.get();
    std::atomic<int> presses{0};

    DebouncedButton button(std::move(source), std::chrono::milliseconds(30), [&] { ++presses; });
    button.start();

    raw_source->press();

    if (!wait_until([&] { return presses.load() == 1; }, std::chrono::milliseconds(500))) {
        fail("expected a single press to trigger the callback once");
    }

    button.stop();
    std::cout << "OK: a single press triggers the callback once\n";
}

void test_bouncy_presses_within_window_count_once() {
    auto source = std::make_unique<FakeGpioEventSource>();
    auto* raw_source = source.get();
    std::atomic<int> presses{0};

    DebouncedButton button(std::move(source), std::chrono::milliseconds(100), [&] { ++presses; });
    button.start();

    raw_source->press();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    raw_source->press();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    raw_source->press();

    // Give the watcher time to process all three edges, then confirm the
    // debounce window collapsed them into a single accepted press.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (presses.load() != 1) {
        fail("expected bouncy presses within the debounce window to count once, got " +
             std::to_string(presses.load()));
    }

    button.stop();
    std::cout << "OK: bouncy presses within the debounce window count as one\n";
}

void test_presses_spaced_beyond_window_count_separately() {
    auto source = std::make_unique<FakeGpioEventSource>();
    auto* raw_source = source.get();
    std::atomic<int> presses{0};

    DebouncedButton button(std::move(source), std::chrono::milliseconds(30), [&] { ++presses; });
    button.start();

    raw_source->press();
    if (!wait_until([&] { return presses.load() == 1; }, std::chrono::milliseconds(500))) {
        fail("expected the first press to register");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    raw_source->press();

    if (!wait_until([&] { return presses.load() == 2; }, std::chrono::milliseconds(500))) {
        fail("expected a press spaced beyond the debounce window to register separately");
    }

    button.stop();
    std::cout << "OK: presses spaced beyond the debounce window count separately\n";
}

void test_stop_halts_further_presses() {
    auto source = std::make_unique<FakeGpioEventSource>();
    auto* raw_source = source.get();
    std::atomic<int> presses{0};

    DebouncedButton button(std::move(source), std::chrono::milliseconds(30), [&] { ++presses; });
    button.start();

    raw_source->press();
    if (!wait_until([&] { return presses.load() == 1; }, std::chrono::milliseconds(500))) {
        fail("expected the press before stop() to register");
    }

    button.stop();
    raw_source->press();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (presses.load() != 1) {
        fail("expected no further presses to register after stop()");
    }

    std::cout << "OK: stop() halts further presses\n";
}

}  // namespace

int main() {
    test_single_press_triggers_once();
    test_bouncy_presses_within_window_count_once();
    test_presses_spaced_beyond_window_count_separately();
    test_stop_halts_further_presses();

    std::cout << "All DebouncedButton checks passed\n";
    return 0;
}
