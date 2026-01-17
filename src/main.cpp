#include <chrono>
#include <iostream>
#include <thread>

#include "ThreadManager.hpp"

int main() {
    ThreadManager pool(4);

    for (int i = 1; i <= 10; ++i) {
        pool.submit([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "Job " << i << " done on thread "
                      << std::this_thread::get_id() << "\n";
        });
    }

    // Wait for jobs to drain (simple demo)
    while (pool.pendingJobs() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    pool.shutdown();
    std::cout << "All jobs finished.\n";
    return 0;
}
