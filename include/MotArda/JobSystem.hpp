/**
 * @file JobSystem.h
 * @brief JobSystem (multi-threads) implementation
 * @author Iván Galán <galansob@esat-alumni.com>
 */


#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

namespace MTRD {
    /**
    * @class JobSystem
    * @brief A class that will manage the threads 
    * @details This class will manage the multiple threads and mutex,
    * being able to queue tasks to them.
    */
    class JobSystem {
    
    public:
        /**
        * @brief Constructor
        * @details Initializes the amount of threads and then places them
        * at the back.
        */
        JobSystem();
        /**
        * @brief Destructor
        * @details Turns the "stop_" boolean to true and notify all threads, 
        * freeing the threads. 
        */
        ~JobSystem();
        
        /**
        * @brief Copy constructor
        * @param JobSystem& right Reference to a JobSystem type object
        * @details We set it to delete because there is no need to copy anything.
        */
        JobSystem(const JobSystem& right) = delete;
        JobSystem& operator=(const JobSystem& right) = delete;

        /**
        * @brief Move constructor
        * @param JobSystem&& right Reference to a JobSystem type object
        * @details We set it to delete because there is no need to copy anything.
        */
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