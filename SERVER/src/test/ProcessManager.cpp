#include "ProcessManager.h"
#include "DaemonFactory.h"
#include "BaseDaemon.h"

ProcessManager* ProcessManager::s_Instance = nullptr;

ProcessManager* ProcessManager::Instance()
{
    if (s_Instance == nullptr)
    {
        s_Instance = new ProcessManager();
    }

    return s_Instance;
}

void ProcessManager::Init()
{
    m_login = DaemonFactory::Create(DaemonType::LOGIN);
    
    m_monitor.AddWatch(m_login->GetPID(), [this]{
        printf("LOGIN DAEMON crashed-> Restart\n");
        m_login->Run();
    });
}

void ProcessManager::StartDaemons()
{
    m_login->Run();
}

void ProcessManager::Monitor()
{
    m_monitor.Loop();
}

void ProcessManager::Restart(const std::string &name)
{

}