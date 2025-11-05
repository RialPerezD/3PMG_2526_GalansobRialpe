#include "MotArda/JobSystem.hpp"

namespace MTRD {

    JobSystem::JobSystem()
        : data_{ std::make_unique<JobSystemData>() } {
        data_->stop_ = false;
        data_->threads = std::thread::hardware_concurrency();

        for (size_t i = 0; i < data_->threads; ++i) {
            data_->workers_.emplace_back(&JobSystem::worker, data_.get());
        }
    }


    JobSystem::~JobSystem() {
        if (data_) {
            {
                std::unique_lock<std::mutex> lock(data_->queue_mutex_);
                data_->stop_ = true;

                data_->condition_.notify_all();
            }
            for (std::thread& worker : data_->workers_) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }
    }


    void JobSystem::enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(data_->queue_mutex_);
            data_->tasks_.push(std::move(task));
        }
        data_->condition_.notify_one();
    }


    void JobSystem::worker(JobSystemData* data) {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(data->queue_mutex_);
                data->condition_.wait(lock, [data]
                    {
                    return data->stop_ || !data->tasks_.empty();
                    });

                if (data->stop_ && data->tasks_.empty()){
                    return;
                    }
                task = std::move(data->tasks_.front());
                data->tasks_.pop();
            }
            task();
        }
    }

}