#include "common.h"
#include "ChannelServer.h"
#include "ConfigLoader.h"
#if 1 /*DB 연결 테스트*/
#include "MySqlConnectionPool.h"
#include "ItemManager.h"
#include "MapManager.h"
#endif

#define DAEMON_NAME "CHANNEL_SERVER"
#define PORT 9001
#define LOG_PATH "../logs/channel"


#if 1
namespace
{
    AppConfig g_config;
}

int main(int ac, char** av)
{
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);

    int channelIndex = 0;
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
            channelIndex = std::atoi(arg.c_str());
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
    MySqlConnectionPool::GetInstance(g_config.mysql);
    ChannelServer channelServer;

    bool start = channelServer.Init(g_config.channelServer.port + channelIndex);
    if (start == false)
    {
        K_slog_close();
        return -1;
    }

    channelServer.Run();

    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", DAEMON_NAME);
    K_slog_close();
    return 0;
}
#else
int main(int ac, char** av)
{
    //log
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);
   
    
    bool Start = false;

    ChannelServer channelServer;

    if (ac == 2)
    {
        Start = channelServer.Init(PORT + atoi(av[1]));
    }
    else
        Start = channelServer.Init(PORT);

    if(Start == false)
    {
        return -1;
    }


    channelServer.Run();

    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", DAEMON_NAME);
    K_slog_close();
    return 0;

}
#endif