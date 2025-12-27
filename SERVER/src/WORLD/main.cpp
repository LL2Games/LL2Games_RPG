#include "WorldServer.h"
#include "K_slog.h"
#include "util/type.h"

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