#include <iostream>
#include <memory>
#include "ProcessMonitor.h"

class BaseDaemon;

class ProcessManager
{
public:
    static ProcessManager* Instance();
    void Init();
    void StartDaemons();
    void Monitor();
    void Restart(const std::string &name);

private:
    ProcessManager() {}
    ~ProcessManager() {}

private:
    static ProcessManager *s_Instance;
    std::unique_ptr<BaseDaemon> m_login, m_chat;
    ProcessMonitor m_monitor;
};