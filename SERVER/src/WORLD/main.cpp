#include "WorldServer.h"
#include "K_slog.h"
#include "util/type.h"
#include "ConfigLoader.h"
namespace
{
    AppConfig g_config;
}

#if 1
int main(int ac, char **av)
{
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);

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
    MySqlConnectionPool::GetInstance(g_config.mysql, 8);
    WorldServer server;

   if (server.Init(g_config.worldServer.port) != 0)
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
int main()
{
    WorldServer server;
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);
    server.Init(PORT);
    server.Run();

    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", DAEMON_NAME);
    K_slog_close();
    return 0;
}
#endif