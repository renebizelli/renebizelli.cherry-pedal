#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

#include "gpio_event_source.hpp"

namespace cherry {

// Polls a GpioEventSource for press edges and invokes on_press for each one
// that is at least `debounce` apart from the last accepted press, filtering
// out the contact bounce a mechanical footswitch produces — the libgpiod
// equivalent of gpiozero.Button's bounce_time.
class DebouncedButton {
public:
    using PressCallback = std::function<void()>;

    DebouncedButton(
        std::unique_ptr<GpioEventSource> source,
        std::chrono::milliseconds debounce,
        PressCallback on_press);

    ~DebouncedButton();

    DebouncedButton(const DebouncedButton&) = delete;
    DebouncedButton& operator=(const DebouncedButton&) = delete;

    void start();
    void stop();

private:
    void watch();

    std::unique_ptr<GpioEventSource> source_;
    std::chrono::milliseconds debounce_;
    PressCallback on_press_;

    std::thread watcher_;
    std::atomic<bool> running_{false};
    std::chrono::steady_clock::time_point last_press_{};
};

}  // namespace cherry
