#include "CHAT/core/Server.h"
#include "slog/K_slog.h"

#define LOG_PATH "../logs/chat"
#define PORT 5005

int main(int ac, char **av)
{
    K_slog_init(LOG_PATH, "CHAT_SERVER");

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

    K_slog_close();
    return 0;
}