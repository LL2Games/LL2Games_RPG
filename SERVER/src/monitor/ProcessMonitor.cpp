#include "ProcessMonitor.h"
#include <sys/stat.h>
#include <stdio.h>

ProcessMonitor::ProcessMonitor()
{}

void ProcessMonitor::AddWatch(int pid, Callback onCrash)
{
    m_items.push_back({pid, onCrash});
}

void ProcessMonitor::Loop()
{
    while(true)
    {
        for (auto& item: m_items)
        {
            struct stat st;
            char procPath[64];
            snprintf(procPath, sizeof(procPath), "/proc/%d", item.pid);

            if (stat(procPath, &st) != 0)
            {
                printf("Crashed[%s]\n", procPath);
                item.cb(); //crash callback
            }
        }
        sleep(LOOP_GAP);
    }
}