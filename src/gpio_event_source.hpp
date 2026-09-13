#pragma once

#include <chrono>

namespace cherry {

// One GPIO line's raw edge detection. wait_for_edge blocks up to `timeout`
// and reports whether a press edge occurred, so a caller polling in a loop
// can also check a "keep running" flag periodically instead of blocking
// forever on a single line.
class GpioEventSource {
public:
    virtual ~GpioEventSource() = default;

    virtual bool wait_for_edge(std::chrono::milliseconds timeout) = 0;
};

}  // namespace cherry
