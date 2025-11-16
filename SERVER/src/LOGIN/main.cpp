#include "core/Server.h"
#include "db/MySQLManager.h"

#define DAEMON_NAME "LOGIN_SERVER"
#define PORT 5000

//log
#include "K_slog.h"
#define LOG_PATH "../logs/login"

int main()
{
    //log
    K_slog_init(LOG_PATH, DAEMON_NAME);

    MySQLManager::Instance().Connect("127.0.0.1", "root", "1234", "testdb", 3306);

    Server server;
    if (!server.Init(PORT))
        return -1;

    server.Run();

    K_slog_close();
    return 0;
}