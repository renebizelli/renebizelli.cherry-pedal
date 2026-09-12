#include "gpiod_event_source.hpp"

#include <stdexcept>

namespace cherry {

GpiodEventSource::GpiodEventSource(const std::string& chip_name, unsigned int line_offset) {
    chip_ = gpiod_chip_open_by_name(chip_name.c_str());
    if (chip_ == nullptr) {
        throw std::runtime_error("Could not open GPIO chip: " + chip_name);
    }

    line_ = gpiod_chip_get_line(chip_, line_offset);
    if (line_ == nullptr) {
        gpiod_chip_close(chip_);
        throw std::runtime_error("Could not get GPIO line " + std::to_string(line_offset));
    }

    gpiod_line_request_config config{};
    config.consumer = "cherry-pedal";
    config.request_type = GPIOD_LINE_REQUEST_EVENT_FALLING_EDGE;
    config.flags = GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP;

    if (gpiod_line_request(line_, &config, 0) != 0) {
        gpiod_chip_close(chip_);
        throw std::runtime_error("Could not request GPIO line " + std::to_string(line_offset));
    }
}

GpiodEventSource::~GpiodEventSource() {
    if (chip_ != nullptr) {
        // Closing the chip also releases the line request.
        gpiod_chip_close(chip_);
    }
}

bool GpiodEventSource::wait_for_edge(std::chrono::milliseconds timeout) {
    const timespec ts{
        timeout.count() / 1000,
        (timeout.count() % 1000) * 1000000,
    };

    if (gpiod_line_event_wait(line_, &ts) <= 0) {
        return false;
    }

    gpiod_line_event event{};
    gpiod_line_event_read(line_, &event);
    return true;
}

}  // namespace cherry
