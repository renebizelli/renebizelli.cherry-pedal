#pragma once

#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "audio_events.hpp"

namespace cherry::testing {

// Thread-safe sink for the AudioEvent callback: PlayerService fires events
// from its background watcher thread, so tests must not read the log
// without synchronization.
class EventRecorder {
public:
    void operator()(AudioEvent event) {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.push_back(event);
    }

    std::vector<AudioEvent> snapshot() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return events_;
    }

private:
    mutable std::mutex mutex_;
    std::vector<AudioEvent> events_;
};

inline void fail(const std::string& message) {
    std::cerr << "FAIL: " << message << "\n";
    std::exit(1);
}

// Polls `predicate` until it is true or `timeout` elapses, for asserting on
// state that changes asynchronously on the watcher thread.
inline bool wait_until(const std::function<bool()>& predicate, std::chrono::milliseconds timeout) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return predicate();
}

}  // namespace cherry::testing
