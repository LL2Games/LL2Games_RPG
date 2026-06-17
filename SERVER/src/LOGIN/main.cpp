#include "common.h"
#include "Server.h"
#include "MySQLManager.h"
#include "ConfigLoader.h"

#if 1
namespace
{
    AppConfig g_config;
}

int main(int ac, char **av)
{
    try
    {
        // log
        K_slog_init(LOGIN_LOG_PATH, LOGIN_DAEMON_NAME);
        K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", LOGIN_DAEMON_NAME);

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
        bool start = server.Init(g_config.loginServer.port);
        if (start == false)
        {
            K_slog_close();
            return -1;
        }

        server.Run();

        K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", LOGIN_DAEMON_NAME);
        K_slog_close();
    }
    catch (const std::exception &ex)
    {
        printf("[%s] Exception: %s\n", LOGIN_DAEMON_NAME, ex.what());
        K_slog_trace(K_SLOG_ERROR, "Exception: %s", ex.what());
        K_slog_close();
        return -1;
    }
    return 0;
}

#else
int main()
{
    // log
    K_slog_init(LOGIN_LOG_PATH, LOGIN_DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", LOGIN_DAEMON_NAME);

    MySQLManager::Instance().Connect("127.0.0.1", "root", "1234", "game", 3306);

    Server server;
    if (!server.Init(PORT))
        return -1;

    server.Run();

    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", LOGIN_DAEMON_NAME);
    K_slog_close();
    return 0;
}
#endif