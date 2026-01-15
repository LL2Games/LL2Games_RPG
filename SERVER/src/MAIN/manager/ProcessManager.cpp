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
    //mainD 데몬으로 재시행
    pid_t pid = fork();
    if (pid < 0) return;

    if (pid > 0) //parent
    {
        exit(0);
    }

    if (pid == 0) //데몬으로 재시행
        return; 
}

void ProcessManager::StartDaemons()
{
    m_login = DaemonFactory::Create(DaemonType::LOGIN);
    m_chat = DaemonFactory::Create(DaemonType::CHAT);
    m_world = DaemonFactory::Create(DaemonType::WORLD);
    m_channel = DaemonFactory::Create(DaemonType::CHANNEL);
    
    m_login->Run();
    m_chat->Run();
    m_world->Run();
    m_channel->Run();

    
    m_monitor.AddWatch(m_login->GetPID(), [this]() -> pid_t{
       printf("LOGIN DAEMON crashed-> Restart\n");
       return m_login->Run();
    });
    
    m_monitor.AddWatch(m_chat->GetPID(), [this]() -> pid_t{
       printf("CHAT_DAEMON crashed-> Restart\n");
       return m_chat->Run();
    });
    
    m_monitor.AddWatch(m_world->GetPID(), [this]() -> pid_t{
        printf("WORLD_DAEMON crashed-> Restart\n");
        return m_world->Run();
    });
    
    m_monitor.AddWatch(m_channel->GetPID(), [this]() -> pid_t{
        printf("CHANNEL_DAEMON crashed-> Restart\n");
        return m_channel->Run();
    });
}

void ProcessManager::Monitor()
{
    m_monitor.Loop();
}
