#pragma once

#include <memory>
#include <vector>

#include "debounced_button.hpp"

namespace cherry {

// Owns one DebouncedButton per configured GPIO pin and starts/stops them
// together.
//
// Keyboard input is intentionally not handled here: unlike the original
// Python (which needed a global keyboard hook via the `keyboard` library so
// shortcuts worked without window focus), the SDL2 UI reads SDL_KEYDOWN
// events directly from its own event loop, since the fullscreen window
// always has focus. See the port plan for this decision.
class InputService {
public:
    void add_button(std::unique_ptr<DebouncedButton> button);

    void start();
    void stop();

private:
    std::vector<std::unique_ptr<DebouncedButton>> buttons_;
};

}  // namespace cherry
