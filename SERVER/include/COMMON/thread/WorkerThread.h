#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Task.h"

class WorkerThread {

public:
    WorkerThread();
    ~WorkerThread();

    void Start();
    void Stop();
    void PushTask(std::unique_ptr<Task> task);

private:
    void Run();

private:
    std::thread m_worker;
    std::queue<std::unique_ptr<Task>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running{false};
};