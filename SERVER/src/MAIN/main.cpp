#include "DaemonFactory.h"
#include "BaseDaemon.h"
#include <iostream>
#include "ProcessManager.h"

int main()
{
    ProcessManager* PM = ProcessManager::Instance();
    PM->Init();
    PM->StartDaemons();
    PM->Monitor();

    return 0;
}