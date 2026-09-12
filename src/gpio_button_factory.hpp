#pragma once

#include <memory>

#include "debounced_button.hpp"

namespace cherry {

// Creates DebouncedButtons bound to real GPIO pins. Kept as its own seam so
// the composition root can wire real hardware while tests wire a fake
// GpioEventSource directly, without either depending on the other.
class GpioButtonFactory {
public:
    virtual ~GpioButtonFactory() = default;

    virtual std::unique_ptr<DebouncedButton> create(
        unsigned int pin, DebouncedButton::PressCallback on_press) const = 0;
};

}  // namespace cherry
