#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>

class ThreadPool 
{
public:
    // 构造函数，初始化线程池
    ThreadPool(size_t threads);
    
    // 将任务添加到线程池中
    void enqueue(std::function<void()> task);
    
    // 等待线程池中的所有任务完成
    void wait_for_completion();
    
    // 析构函数，确保所有线程完成任务
    ~ThreadPool();

private:
    std::vector<std::thread> workers; // 工作线程
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queue_mutex; // 保护任务队列的互斥量
    std::condition_variable condition; // 条件变量，用于线程同步
    bool stop = false; // 标记线程池是否停止
};
