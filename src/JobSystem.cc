#include "MotArda/JobSystem.hpp"

namespace MTRD {

    JobSystem::JobSystem(size_t threads) : stop_(false) {
        for (size_t i = 0; i < threads; ++i) {
            workers_.emplace_back(&JobSystem::worker, this);
        }
    }

    JobSystem::~JobSystem() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread& worker : workers_) {
            if (worker.joinable())
                worker.join();
        }
    }

    void JobSystem::enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.push(std::move(task));
        }
        condition_.notify_one();
    }

    void JobSystem::worker() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                condition_.wait(lock, [this] 
                    {
                    return stop_ || !tasks_.empty();
                    });

                if (stop_ && tasks_.empty())
                    return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }
}