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
        /**
        * @brief Copy assignment
        * @param JobSystem& right Reference to a JobSystem type object
        * @details We set it to delete because there is no need to copy anything.
        */
        JobSystem& operator=(const JobSystem& right) = delete;

        /**
        * @brief Move constructor
        * @param JobSystem&& right Reference to a JobSystem type object
        * @details We are able to move threads as unique_ptr allows it.
        */
        JobSystem(JobSystem&& right) = default;
        /**
        * @brief Move assignment
        * @param JobSystem&& right Reference to a JobSystem type object
        * @details We are able to move threads as unique_ptr allows it.
        */
        JobSystem& operator=(JobSystem&& right) = default;

        /** 
        * @brief Adds a task to the queue
        * @param std::function<void()> task The function we will we adding to the queue
        * @details Adds a task to the queue by blocking the mutex and waking up
        * a thread.
        */
        void enqueue(std::function<void()> task);


    private:
        /**
         * @struct JobSystemData
         * @brief Internal data structure used by JobSystem.
         * @details Stores the worker threads, the task queue, synchronization
         * primitives and state used by the JobSystem thread pool.
         * Additionally, we make the struct's object as unique_ptr so
         * we can move or copy the content.
         */
        struct JobSystemData {
            //< Vector that contains the worker threads.
            std::vector<std::thread> workers_;
            //< Queue of future functions to be processed by workers.
            std::queue<std::function<void()>> tasks_;

            //< Mutex prevents shared data from being simultaneously 
            //< accessed by multiple threads.
            std::mutex queue_mutex_;

            std::condition_variable condition_;
            //< The amount of threads the JobSystem will manage.
            size_t threads;

            //< Boolean variable that allow workers to free.
            std::atomic<bool> stop_;
        };

        /**
        * @brief Manage the threads behavior
        * @param JobSystemData* data --------------------
        * @details Creates an infinite loop until "stop_" is true or the function
        * finds a task. 
        * The function locks the mutex to be able to check the queue
        */
        static void worker(JobSystemData* data);

        std::unique_ptr<JobSystemData> data_;

    };

} 