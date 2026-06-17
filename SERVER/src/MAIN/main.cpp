#include "DaemonFactory.h"
#include "BaseDaemon.h"
#include <iostream>
#include <signal.h>
#include "ProcessManager.h"
#include "common.h"

void CleanChildren(int sig)
{
    ProcessManager* PM = ProcessManager::Instance();
    for (const auto &item : PM->GetMonitor().GetItems())
    {
        kill(item.pid, sig);
    }
    exit(0);
}
    
int main()
{
    //signal
    signal(SIGKILL, CleanChildren);
    signal(SIGTERM, CleanChildren);
    signal(SIGINT, CleanChildren);

    //log
    K_slog_init(MAIN_LOG_PATH, MAIN_DAEMON_NAME);
    ProcessManager* PM = ProcessManager::Instance();
    PM->Init();
    PM->StartDaemons();
    PM->Monitor();

    K_slog_close();
    return 0;
}