#include "input_service.hpp"

namespace cherry {

void InputService::add_button(std::unique_ptr<DebouncedButton> button) {
    buttons_.push_back(std::move(button));
}

void InputService::start() {
    for (auto& button : buttons_) {
        button->start();
    }
}

void InputService::stop() {
    for (auto& button : buttons_) {
        button->stop();
    }
}

}  // namespace cherry
