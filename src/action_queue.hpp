#pragma once

#include <functional>
#include <mutex>
#include <queue>

namespace cherry {

// Thread-safe queue of actions to run on the main/render thread.
//
// GPIO button presses fire on DebouncedButton's background watcher
// threads, but screen switching and anything touching SDL must only
// happen on the thread that owns the SDL renderer — so GPIO callbacks
// push an action here instead of calling into a screen directly, and the
// main loop drains the queue once per frame before rendering. Keyboard
// input needs no such marshaling: it is already read on the main thread
// via the SDL event loop.
class ActionQueue {
public:
    void push(std::function<void()> action);
    void drain();

private:
    std::mutex mutex_;
    std::queue<std::function<void()>> actions_;
};

}  // namespace cherry
