#pragma once

#include <string>

#include <gpiod.h>

#include "gpio_event_source.hpp"

namespace cherry {

// Wraps one GPIO line configured with an internal pull-up, watching for
// falling-edge events — a footswitch pulls the line low when pressed. This
// is the libgpiod equivalent of gpiozero.Button(pull_up=True); sysfs GPIO
// (what older Python stacks sometimes used) is deprecated on current
// Raspberry Pi OS, so libgpiod is the supported path.
class GpiodEventSource : public GpioEventSource {
public:
    GpiodEventSource(const std::string& chip_name, unsigned int line_offset);
    ~GpiodEventSource() override;

    GpiodEventSource(const GpiodEventSource&) = delete;
    GpiodEventSource& operator=(const GpiodEventSource&) = delete;

    bool wait_for_edge(std::chrono::milliseconds timeout) override;

private:
    gpiod_chip* chip_ = nullptr;
    gpiod_line* line_ = nullptr;
};

}  // namespace cherry
