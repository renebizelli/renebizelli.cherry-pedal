#include "action_queue.hpp"

namespace cherry {

void ActionQueue::push(std::function<void()> action) {
    std::lock_guard<std::mutex> lock(mutex_);
    actions_.push(std::move(action));
}

void ActionQueue::drain() {
    std::queue<std::function<void()>> pending;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::swap(pending, actions_);
    }

    while (!pending.empty()) {
        pending.front()();
        pending.pop();
    }
}

}  // namespace cherry
