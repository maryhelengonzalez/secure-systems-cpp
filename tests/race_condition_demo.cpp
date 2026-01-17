#include <iostream>
#include <thread>
#include <vector>

int counter = 0;  // NOT thread-safe

void increment() {
    for (int i = 0; i < 100000; ++i) {
        counter++;  // race condition here
    }
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(increment);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << counter << std::endl;
    std::cout << "Expected value: 400000" << std::endl;

    return 0;
}
