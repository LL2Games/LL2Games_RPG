#include "CHAT/core/Server.h"
#include "db/MySQLManager.h"
#include "ConfigLoader.h"

#include "K_slog.h"

#define DAEMON_NAME "CHAT_SERVER"
#define LOG_PATH "../logs/chat"
#define PORT 9101


#if 1
namespace
{
    AppConfig g_config;
}

int main(int ac, char **av)
{
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);

    int chatIndex = 0;
    std::string configPath;

    for (int i = 1; i < ac; ++i)
    {
        std::string arg = av[i];

        if (arg == "--config")
        {
            if (i + 1 >= ac)
            {
                K_slog_trace(K_SLOG_ERROR, "Missing config path after --config");
                K_slog_close();
                return -1;
            }

            configPath = av[++i];
        }
        else
        {
            chatIndex = std::atoi(arg.c_str());
        }
    }

    if (configPath.empty())
    {
        K_slog_trace(K_SLOG_ERROR, "Missing required --config argument");
        K_slog_close();
        return -1;
    }

    ConfigLoader loader;
    if (!loader.Load(configPath))
    {
        K_slog_trace(K_SLOG_ERROR, "Failed to load config: %s", configPath.c_str());
        K_slog_close();
        return -1;
    }

    g_config = loader.ToAppConfig();

    if (!MySQLManager::Instance().Connect(
                g_config.mysql.host.c_str(),
                g_config.mysql.user.c_str(),
                g_config.mysql.password.c_str(),
                g_config.mysql.database.c_str(),
                g_config.mysql.port))
    {
        K_slog_trace(K_SLOG_ERROR, "Failed to connect MySQL");
        K_slog_close();
        return -1;
    }

    Server server;

    bool start = server.Init(g_config.chatServer.port + chatIndex);
    if (start == false)
    {
        K_slog_close();
        return -1;
    }

    server.Run();

    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", DAEMON_NAME);
    K_slog_close();
    return 0;
}

#else
int main(int ac, char **av)
{
    K_slog_init(LOG_PATH, "CHAT_SERVER");
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);

    MySQLManager::Instance().Connect("127.0.0.1", "root", "1234", "game", 3306);

    Server server;

    if (ac == 2)
    {
        if (!server.Init(PORT + atoi(av[1])))
            return -1;
    }
    else
    {
        if (!server.Init(PORT))
            return -1;
    }
    
    
    server.Run();
    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", DAEMON_NAME);
    K_slog_close();
    return 0;
}
#endif