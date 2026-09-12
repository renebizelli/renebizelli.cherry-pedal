#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "gpio_button_factory.hpp"

namespace cherry {

// Production GpioButtonFactory: every button reads a real GPIO line via
// libgpiod, pull-up enabled, with the given software debounce window
// (matches the 30 ms bounce_time the original app used).
class GpiodButtonFactory : public GpioButtonFactory {
public:
    explicit GpiodButtonFactory(
        std::chrono::milliseconds debounce = std::chrono::milliseconds(30),
        std::string chip_name = "gpiochip0");

    std::unique_ptr<DebouncedButton> create(
        unsigned int pin, DebouncedButton::PressCallback on_press) const override;

private:
    std::chrono::milliseconds debounce_;
    std::string chip_name_;
};

}  // namespace cherry
