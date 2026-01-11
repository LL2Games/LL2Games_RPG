#include "COMMON/common.h"
#include "CHANNEL/core/ChannelServer.h"

#if 1 /*DB 연결 테스트*/
#include "CHANNEL/db/MySqlConnectionPool.h"
#endif

#define DAEMON_NAME "CHANNEL_SERVER"
#define PORT 1234
#define LOG_PATH "../logs/channel"


int main()
{
    //log
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);
   
    bool Start = false;
    ChannelServer channelServer;

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