#include "common.h"
#include "ChannelServer.h"

#if 1 /*DB 연결 테스트*/
#include "MySqlConnectionPool.h"
#include "ItemManager.h"
#include "MapManager.h"
#endif

#define DAEMON_NAME "CHANNEL_SERVER"
#define PORT 1234
#define LOG_PATH "../logs/channel"


int main(int ac, char** av)
{
    //log
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);
   
    
#if 0 /*LJH TEST */
    MapManager map_manager;
    map_manager.Init();

  

#else
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
#endif
   


    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", DAEMON_NAME);
    K_slog_close();
    return 0;

}