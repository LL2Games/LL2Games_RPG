#pragma once
#include <functional>
#include <vector>
#include <unistd.h>

#define LOOP_GAP 10

class ProcessMonitor
{
public:
    ProcessMonitor();
    using Callback = std::function<void()>;

    void AddWatch(int pid, Callback onCrash);
    void Loop();

private:
    struct WatchItem {
        int pid;
        Callback cb;
    };
    std::vector<WatchItem> m_items;
};