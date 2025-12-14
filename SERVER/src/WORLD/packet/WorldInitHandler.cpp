#include "WorldInitHandler.h"
#include "K_slog.h"
#include "WorldSession.h"

void WorldInitHandler::Execute(PacketContext *ctx)
{
    std::string id(ctx->payload, ctx->payload_len);
    WorldSession *session = ctx->session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        return;
    }

    session->SetAccountid(id);
}