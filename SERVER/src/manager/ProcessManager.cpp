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
    m_chat = DaemonFactory::Create(DaemonType::CHAT);
}

void ProcessManager::StartDaemons()
{
    m_login->Run();
    m_chat->Run();

    m_monitor.AddWatch(m_login->GetPID(), [this]{
        printf("LOGIN DAEMON crashed-> Restart\n");
        m_login->Run();
    });
    m_monitor.AddWatch(m_chat->GetPID(), [this]{
        printf("CHAT_DAEMON crashed-> Restart\n");
        m_chat->Run();
    });
}

void ProcessManager::Monitor()
{
    m_monitor.Loop();
}
