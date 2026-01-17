#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "ThreadManager.hpp"

static void test_jobs_execute() {
    ThreadManager pool(4);

    std::atomic<int> counter{0};

    for (int i = 0; i < 50; ++i) {
        pool.submit([&counter] {
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }

    // Wait (briefly) for all jobs to complete
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (pool.pendingJobs() > 0 && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    pool.shutdown();

    assert(counter.load(std::memory_order_relaxed) == 50 && "Expected all jobs to execute");
}

static void test_submit_after_shutdown_throws() {
    ThreadManager pool(2);
    pool.shutdown();

    bool threw = false;
    try {
        pool.submit([] {});
    } catch (const std::runtime_error&) {
        threw = true;
    } catch (...) {
        // still counts as "throws" but we'd prefer runtime_error
        threw = true;
    }

    assert(threw && "Expected submit() after shutdown to throw");
}

int main() {
    std::cout << "[TEST] test_jobs_execute...\n";
    test_jobs_execute();
    std::cout << "[PASS] test_jobs_execute\n";

    std::cout << "[TEST] test_submit_after_shutdown_throws...\n";
    test_submit_after_shutdown_throws();
    std::cout << "[PASS] test_submit_after_shutdown_throws\n";

    std::cout << "ALL TESTS PASSED\n";
    return 0;
}
