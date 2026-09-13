#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "gpio_button_factory.hpp"

namespace cherry {

// Production GpioButtonFactory: every button reads a real GPIO line via
// libgpiod, pull-up enabled, with the given software debounce window.
//
// The original Python app used gpiozero's default bounce_time of 30ms, but
// monitoring the actual footswitches on the real Raspberry Pi with gpiomon
// showed a single physical press/release producing mechanical contact
// bounce spanning up to ~190ms — well past a 30ms window — which was
// misread as a second, independent press (typically on release). 200ms
// comfortably covers the observed bounce with margin and is still well
// under what a person perceives as input lag.
class GpiodButtonFactory : public GpioButtonFactory {
public:
    explicit GpiodButtonFactory(
        std::chrono::milliseconds debounce = std::chrono::milliseconds(200),
        std::string chip_name = "gpiochip0");

    std::unique_ptr<DebouncedButton> create(
        unsigned int pin, DebouncedButton::PressCallback on_press) const override;

private:
    std::chrono::milliseconds debounce_;
    std::string chip_name_;
};

}  // namespace cherry
