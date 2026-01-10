#include "WorldInitHandler.h"
#include "K_slog.h"
#include "WorldSession.h"
#include "PacketParser.h"

void WorldInitHandler::Execute(PacketContext *ctx)
{
    WorldSession *session = nullptr;
    size_t offset = 0;
    std::string account_id;
    int rc = EXIT_SUCCESS;
    std::string errMsg;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            account_id,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }

    K_slog_trace(K_SLOG_DEBUG,
                 "[%s] client(%d) account_id=[%s]",
                 __FUNCTION__, session->GetFD(), account_id.c_str());

    session->SetAccountid(account_id);

err:
    if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_INIT_WORLD, errMsg);
    }
    else
        session->SendOk(PKT_INIT_WORLD);
}