#pragma once

#include "models.hpp"

namespace cherry {

// Lets a screen ask to switch to another screen or quit, without knowing
// about the other screens or how the application is composed.
class Navigator {
public:
    virtual ~Navigator() = default;

    virtual void show_setup() = 0;
    virtual void show_panel(const Band& band) = 0;
    virtual void show_sync(const Band& band) = 0;
    virtual void quit() = 0;
    virtual void request_restart() = 0;
};

}  // namespace cherry
