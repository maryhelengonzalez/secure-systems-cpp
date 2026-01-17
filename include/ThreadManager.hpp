#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadManager {
public:
    explicit ThreadManager(std::size_t workerCount);
    ~ThreadManager();

    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

    // Add a job to be executed by the worker pool.
    void submit(std::function<void()> job);

    // Stop accepting new jobs, finish queued jobs, and join threads.
    void shutdown();

    std::size_t pendingJobs() const;

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<std::function<void()>> jobs_;

    std::atomic<bool> stopping_{false};
    std::atomic<std::size_t> pending_{0};
};
