#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadCount)
{
    if (threadCount == 0)
    {
        threadCount = 4; // 기본값으로 4개의 스레드를 사용
    }

    m_poolSize = threadCount;

    for(size_t i = 0; i < threadCount; i++)
    {
        m_workers.emplace_back(std::make_unique<WorkerThread>());
    }
}

ThreadPool::~ThreadPool()
{
    Stop();
}


void ThreadPool::Start()
{
    for (auto& worker : m_workers)
    {
        worker->Start();
    }
}

void ThreadPool::Stop()
{
    for (auto& worker : m_workers)
    {
        worker->Stop();
    }
}

void ThreadPool::Submit(std::unique_ptr<Task> task)
{
    m_workers[m_nextIndex]->PushTask(std::move(task));
    m_nextIndex = (m_nextIndex + 1) % m_workers.size();
}
