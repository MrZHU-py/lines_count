#include "thread_pool.h"
#include <iostream>

ThreadPool::ThreadPool(size_t threads) 
{
    // 创建指定数量的工作线程
    for (size_t i = 0; i < threads; ++i) 
    {
        workers.emplace_back([this] 
        {
            for (;;) 
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    // 等待条件变量通知或线程池停止
                    this->condition.wait(lock, [this] 
                    { 
                        return this->stop || !this->tasks.empty(); 
                    });
                    
                    if (this->stop && this->tasks.empty()) 
                    {
                        return;
                    }
                    
                    // 获取并移除任务
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                
                try 
                {
                    task(); // 执行任务
                } 
                catch (const std::exception& e) 
                {
                    std::cerr << "线程执行错误: " << e.what() << std::endl;
                }
            }
        });
    }
}

void ThreadPool::enqueue(std::function<void()> task) 
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) 
        {
            throw std::runtime_error("线程池已停止，不能再添加任务");
        }
        tasks.push(std::move(task));
    }
    condition.notify_one(); // 通知一个等待的线程
}

void ThreadPool::wait_for_completion() 
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true; // 停止接收新任务
    }
    condition.notify_all(); // 通知所有等待的线程
    
    // 等待所有工作线程完成
    for (std::thread &worker : workers) 
    {
        worker.join();
    }
}

ThreadPool::~ThreadPool() 
{
    wait_for_completion(); // 确保所有线程在析构时完成
}
