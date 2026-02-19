#include "WorkerThread.h"

WorkerThread::WorkerThread()
{

}
WorkerThread::~WorkerThread()
{
    Stop();
}

void WorkerThread::Start()
{
    m_running = true;
    m_worker = std::thread(&WorkerThread::Run, this);
}

void WorkerThread::Stop()
{
    m_running = false;
    m_cv.notify_all();
    if (m_worker.joinable())
        m_worker.join();
}

void WorkerThread::PushTask(std::unique_ptr<Task> task)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(task));
    }
    m_cv.notify_one();
}

void WorkerThread::Run()
{
    while (m_running)
    {
        std::unique_ptr<Task> task;

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [&]() {
                return !m_tasks.empty() || !m_running;
            });

            if (!m_running) break;

            task = std::move(m_tasks.front());
            m_tasks.pop();
        }

        if (task)
            task->Execute();
    }
}
