#include "gpiod_button_factory.hpp"

#include "gpiod_event_source.hpp"

namespace cherry {

GpiodButtonFactory::GpiodButtonFactory(std::chrono::milliseconds debounce, std::string chip_name)
    : debounce_(debounce), chip_name_(std::move(chip_name)) {}

std::unique_ptr<DebouncedButton> GpiodButtonFactory::create(
    unsigned int pin, DebouncedButton::PressCallback on_press) const {
    return std::make_unique<DebouncedButton>(
        std::make_unique<GpiodEventSource>(chip_name_, pin), debounce_, std::move(on_press));
}

}  // namespace cherry
