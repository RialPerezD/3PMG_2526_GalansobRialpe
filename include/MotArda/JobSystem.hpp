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
        void worker();

        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;

        //Estos dos no pueden copiarse ni moverse, por eso son unique_ptr
        std::unique_ptr<std::mutex> queue_mutex_;
        std::unique_ptr<std::condition_variable> condition_;

        size_t threads;

        bool stop_;
    };

} 