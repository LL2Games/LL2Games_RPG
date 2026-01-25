#include "ProcessManager.h"
#include "DaemonFactory.h"
#include "BaseDaemon.h"
#include "K_slog.h"

#define CHANNEL_COUNT 3
ProcessManager *ProcessManager::s_Instance = nullptr;

ProcessManager *ProcessManager::Instance()
{
    if (s_Instance == nullptr)
    {
        s_Instance = new ProcessManager();
    }

    return s_Instance;
}

void ProcessManager::Init()
{
    // mainD 데몬으로 재시행
    pid_t pid = fork();
    if (pid < 0)
        return;

    if (pid > 0) // parent
    {
        exit(0);
    }

    if (pid == 0) // 데몬으로 재시행
        return;
}

void ProcessManager::StartDaemons()
{
    
    m_login = DaemonFactory::Create(DaemonType::LOGIN);
    m_world = DaemonFactory::Create(DaemonType::WORLD);

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        m_chats.push_back(DaemonFactory::Create(DaemonType::CHAT, i));
        m_channels.push_back(DaemonFactory::Create(DaemonType::CHANNEL, i));
    }

    m_login->Run();
    m_world->Run();

    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]m_chats.size[%d]", __FILE__, __LINE__, m_chats.size());
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]m_channels.size[%d]", __FILE__, __LINE__, m_channels.size());


    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        std::unique_ptr<BaseDaemon> &m_chat = m_chats[i];
        std::unique_ptr<BaseDaemon> &m_channel = m_channels[i];

        m_chat->Run(i);
        m_channel->Run(i);
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]m_channels->Run(%d)", __FILE__, __LINE__, i);
    }

    m_monitor.AddWatch(m_login->GetPID(), [this]() -> pid_t
                       {
       printf("LOGIN DAEMON crashed-> Restart\n");
       return m_login->Run(); });

    m_monitor.AddWatch(m_world->GetPID(), [this]() -> pid_t
                       {
        printf("WORLD_DAEMON crashed-> Restart\n");
        return m_world->Run(); });

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        int idx = i;

        m_monitor.AddWatch(m_chats[idx]->GetPID(), [this, idx]() -> pid_t
                           {
       printf("CHAT_DAEMON crashed-> Restart\n");
       return m_chats[idx]->Run(idx); });
        m_monitor.AddWatch(m_channels[idx]->GetPID(), [this, idx]() -> pid_t
                           {
        printf("CHANNEL_DAEMON crashed-> Restart\n");
        return m_channels[idx]->Run(idx); });
    }
}

void ProcessManager::Monitor()
{
    m_monitor.Loop();
}
