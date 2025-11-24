#include "CHAT/core/Server.h"
#include "K_slog.h"

#define DAEMON_NAME "CHAT_SERVER"
#define LOG_PATH "../logs/chat"
#define PORT 5200

int main(int ac, char **av)
{
    K_slog_init(LOG_PATH, "CHAT_SERVER");
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);

    Server server;

    if (ac == 2)
    {
        if (!server.Init(atoi(av[1])))
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