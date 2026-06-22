#pragma once
#include <memory>
#include <vector>
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
    std::unique_ptr<BaseDaemon> m_login;
    std::unique_ptr<BaseDaemon> m_world;


    std::vector<std::unique_ptr<BaseDaemon>> m_channels;
    std::vector<std::unique_ptr<BaseDaemon>> m_chats;

    ProcessMonitor m_monitor;

public:
    ProcessMonitor& GetMonitor() { return m_monitor; }
};