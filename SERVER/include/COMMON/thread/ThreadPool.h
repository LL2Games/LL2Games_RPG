#pragma once
#include <vector>
#include <memory>
#include "WorkerThread.h"

class ThreadPool {

public:
    explicit ThreadPool(size_t threadCount);
    ~ThreadPool();

    void Start();
    void Stop();
    void Submit(std::unique_ptr<Task> task);

private:
    std::vector<std::unique_ptr<WorkerThread>> m_workers;
    size_t m_nextIndex{0};

};