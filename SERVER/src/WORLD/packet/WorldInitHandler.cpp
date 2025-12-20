#include "WorldInitHandler.h"
#include "K_slog.h"
#include "WorldSession.h"

void WorldInitHandler::Execute(PacketContext *ctx)
{
    WorldSession *session = nullptr;
    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        return;
    }
    session = ctx->session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        return;
    }

    const char* data = ctx->payload;
    size_t len = ctx->payload_len;

    size_t offset = 0;
    std::string account_id;

    // payload 파싱
    if (offset >= len)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] payload empty", __FUNCTION__);
        return;
    }

    // length
    uint8_t value_len = static_cast<uint8_t>(data[offset]);
    offset += 1;

    // (선택) 예약 바이트 skip
    if (offset < len && data[offset] == 0x00)
        offset += 1;

    if (offset + value_len > len)
    {
        K_slog_trace(K_SLOG_ERROR,
                     "[%s] payload length overflow", __FUNCTION__);
        return;
    }

    // value
    account_id.assign(data + offset, value_len);
    offset += value_len;

    K_slog_trace(K_SLOG_DEBUG,
        "[%s] client(%d) account_id=[%s]",
        __FUNCTION__, session->GetFD(), account_id.c_str());

    session->SetAccountid(account_id);

}