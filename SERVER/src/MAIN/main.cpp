#include "DaemonFactory.h"
#include "BaseDaemon.h"
#include <iostream>
#include "ProcessManager.h"
#include "K_slog.h"

//log
#include "K_slog.h"
#define LOG_PATH "../logs/main"
#define DAEMON_NAME "MAIN_SERVER"
    
int main()
{
    //log
    K_slog_init(LOG_PATH, DAEMON_NAME);
    ProcessManager* PM = ProcessManager::Instance();
    PM->Init();
    PM->StartDaemons();
    PM->Monitor();

    K_slog_close();
    return 0;
}