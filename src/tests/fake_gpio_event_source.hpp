#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "gpio_event_source.hpp"

namespace cherry::testing {

// Lets tests simulate GPIO press edges — including rapid, bouncy ones —
// without real hardware. Each press() call queues one edge for
// wait_for_edge() to consume.
class FakeGpioEventSource : public cherry::GpioEventSource {
public:
    void press() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++pending_edges_;
        condition_.notify_all();
    }

    bool wait_for_edge(std::chrono::milliseconds timeout) override {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!condition_.wait_for(lock, timeout, [&] { return pending_edges_ > 0; })) {
            return false;
        }

        --pending_edges_;
        return true;
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    int pending_edges_ = 0;
};

}  // namespace cherry::testing
