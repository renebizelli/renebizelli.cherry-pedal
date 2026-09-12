#include "debounced_button.hpp"

namespace cherry {

namespace {
constexpr auto kWaitTimeout = std::chrono::milliseconds(50);
}

DebouncedButton::DebouncedButton(
    std::unique_ptr<GpioEventSource> source,
    std::chrono::milliseconds debounce,
    PressCallback on_press)
    : source_(std::move(source)), debounce_(debounce), on_press_(std::move(on_press)) {}

DebouncedButton::~DebouncedButton() {
    stop();
}

void DebouncedButton::start() {
    if (running_.exchange(true)) {
        return;
    }

    watcher_ = std::thread(&DebouncedButton::watch, this);
}

void DebouncedButton::stop() {
    if (!running_.exchange(false)) {
        return;
    }

    if (watcher_.joinable()) {
        watcher_.join();
    }
}

void DebouncedButton::watch() {
    while (running_.load()) {
        if (!source_->wait_for_edge(kWaitTimeout)) {
            continue;
        }

        const auto now = std::chrono::steady_clock::now();
        if (now - last_press_ < debounce_) {
            continue;
        }

        last_press_ = now;
        on_press_();
    }
}

}  // namespace cherry
