#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "action_queue.hpp"
#include "test_support.hpp"

using namespace cherry;
using cherry::testing::fail;

namespace {

void test_drain_runs_pushed_actions_in_order() {
    ActionQueue queue;
    std::vector<int> order;

    queue.push([&] { order.push_back(1); });
    queue.push([&] { order.push_back(2); });
    queue.push([&] { order.push_back(3); });

    queue.drain();

    if (order != std::vector<int>{1, 2, 3}) {
        fail("expected drain() to run pushed actions in order");
    }

    std::cout << "OK: drain() runs pushed actions in order\n";
}

void test_drain_with_nothing_pending_is_a_no_op() {
    ActionQueue queue;
    queue.drain();
    std::cout << "OK: drain() with nothing pending is a no-op\n";
}

void test_actions_pushed_from_other_threads_are_drained() {
    ActionQueue queue;
    std::atomic<int> counter{0};

    std::vector<std::thread> pushers;
    for (int i = 0; i < 8; ++i) {
        pushers.emplace_back([&] { queue.push([&] { ++counter; }); });
    }
    for (auto& t : pushers) {
        t.join();
    }

    queue.drain();

    if (counter.load() != 8) {
        fail("expected all actions pushed from other threads to be drained");
    }

    std::cout << "OK: actions pushed from other threads are drained\n";
}

}  // namespace

int main() {
    test_drain_runs_pushed_actions_in_order();
    test_drain_with_nothing_pending_is_a_no_op();
    test_actions_pushed_from_other_threads_are_drained();

    std::cout << "All ActionQueue checks passed\n";
    return 0;
}
