#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

std::atomic<int> counter{0};

void increment() {
    for (int i = 0; i < 100000; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(increment);
    for (auto& t : threads) t.join();

    std::cout << "Final counter value: " << counter.load() << "\n";
    std::cout << "Expected value: 400000\n";
    return (counter.load() == 400000) ? 0 : 1;
}
