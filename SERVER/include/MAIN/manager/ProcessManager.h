#pragma once
#include <memory>
#include "DaemonFactory.h"
#include "ProcessMonitor.h"

class BaseDaemon;

//Singleton

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
    std::unique_ptr<BaseDaemon> m_login, m_chat, m_world;
    ProcessMonitor m_monitor;

public:
    ProcessMonitor& GetMonitor() { return m_monitor; }
};