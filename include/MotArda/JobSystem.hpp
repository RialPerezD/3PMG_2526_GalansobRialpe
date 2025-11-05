#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

namespace MTRD {

    class JobSystem {
  
    public:
        JobSystem();
        ~JobSystem();

        JobSystem(const JobSystem& right) = delete;
        JobSystem& operator=(const JobSystem& right) = delete;

        JobSystem(JobSystem&& right) = default;
        JobSystem& operator=(JobSystem&& right) = default;

        void enqueue(std::function<void()> task);


    private:
        struct JobSystemData {
            std::vector<std::thread> workers_;
            std::queue<std::function<void()>> tasks_;

            std::mutex queue_mutex_;
            std::condition_variable condition_;
            size_t threads;

            std::atomic<bool> stop_;
        };

        static void worker(JobSystemData* data);

        std::unique_ptr<JobSystemData> data_;

    };

} 