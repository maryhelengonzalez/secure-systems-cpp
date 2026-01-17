#include "ThreadManager.hpp"

#include <stdexcept>

ThreadManager::ThreadManager(std::size_t workerCount) {
    if (workerCount == 0) {
        throw std::invalid_argument("workerCount must be > 0");
    }

    workers_.reserve(workerCount);
    for (std::size_t i = 0; i < workerCount; ++i) {
        workers_.emplace_back(&ThreadManager::workerLoop, this);
    }
}

ThreadManager::~ThreadManager() {
    shutdown();
}

void ThreadManager::submit(std::function<void()> job) {
    if (!job) {
        throw std::invalid_argument("job must be callable");
    }

    if (stopping_.load()) {
        throw std::runtime_error("ThreadManager is shutting down");
    }

    {
        std::lock_guard<std::mutex> lock(mtx_);
        jobs_.push(std::move(job));
        ++pending_;
    }
    cv_.notify_one();
}

void ThreadManager::shutdown() {
    bool expected = false;
    if (!stopping_.compare_exchange_strong(expected, true)) {
        // already shutting down
        return;
    }

    cv_.notify_all();

    for (auto& t : workers_) {
        if (t.joinable()) t.join();
    }
    workers_.clear();
}

std::size_t ThreadManager::pendingJobs() const {
    return pending_.load();
}

void ThreadManager::workerLoop() {
    while (true) {
        std::function<void()> job;

        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&] {
                return stopping_.load() || !jobs_.empty();
            });

            if (stopping_.load() && jobs_.empty()) {
                return; // graceful exit
            }

            job = std::move(jobs_.front());
            jobs_.pop();
        }

        // Execute outside lock
        try {
            job();
        } catch (...) {
            // In real systems you would log this instead of crashing the worker.
        }

        --pending_;
    }
}
