#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "debounced_button.hpp"
#include "fake_gpio_event_source.hpp"
#include "input_service.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::fail;
using cherry::testing::FakeGpioEventSource;
using cherry::testing::wait_until;

namespace {

void test_input_service_starts_and_stops_all_buttons_together() {
    auto source_a = std::make_unique<FakeGpioEventSource>();
    auto source_b = std::make_unique<FakeGpioEventSource>();
    auto* raw_a = source_a.get();
    auto* raw_b = source_b.get();

    std::atomic<int> presses_a{0};
    std::atomic<int> presses_b{0};

    InputService service;
    service.add_button(std::make_unique<DebouncedButton>(
        std::move(source_a), std::chrono::milliseconds(30), [&] { ++presses_a; }));
    service.add_button(std::make_unique<DebouncedButton>(
        std::move(source_b), std::chrono::milliseconds(30), [&] { ++presses_b; }));

    service.start();

    raw_a->press();
    raw_b->press();

    if (!wait_until(
            [&] { return presses_a.load() == 1 && presses_b.load() == 1; },
            std::chrono::milliseconds(500))) {
        fail("expected both buttons to react to presses after start()");
    }

    service.stop();

    raw_a->press();
    raw_b->press();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (presses_a.load() != 1 || presses_b.load() != 1) {
        fail("expected no further presses to register after stop()");
    }

    std::cout << "OK: InputService starts and stops all buttons together\n";
}

}  // namespace

int main() {
    test_input_service_starts_and_stops_all_buttons_together();

    std::cout << "All InputService checks passed\n";
    return 0;
}
