#include "packet/ChannelSelectHandler.h"
#include "WorldSession.h"
#include "ChannelManager.h"
#include "K_slog.h"


void ChannelSelectHandler::Execute(PacketContext* ctx)
{
    WorldSession *session = nullptr;
    ChannelManager *channel_manager = nullptr;
    int rc = EXIT_SUCCESS;
    std::string errMsg;
    const char* data = nullptr;
    size_t len = 0;
    size_t offset = 0;
    uint8_t value_len = 0;
    std::string channel_id;

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
    channel_manager = ctx->channel_manager;
    if (channel_manager == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] channel_manager is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]channel_manager is nullptr";
        goto err;
    }
    
    data = ctx->payload;
    len = ctx->payload_len;

    // payload 파싱
    if (offset >= len)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] payload empty", __FUNCTION__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]payload empty";
        goto err;
    }

    // length
    value_len = static_cast<uint8_t>(data[offset]);
    offset += 1;

    // (선택) 예약 바이트 skip
    if (offset < len && data[offset] == 0x00)
        offset += 1;

    if (offset + value_len > len)
    {
        K_slog_trace(K_SLOG_ERROR,
                     "[%s] payload length overflow", __FUNCTION__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]payload length overflow";
        goto err;
    }

    // value
    channel_id.assign(data + offset, value_len);
    offset += value_len;

    K_slog_trace(K_SLOG_DEBUG,
        "[%s] client(%d) channel_id=[%s]",
        __FUNCTION__, session->GetFD(), channel_id.c_str());

err:
    if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_SELECT_CHANNEL, errMsg);
    }
    else
    {
        auto opt = channel_manager->SelectChannel(channel_id);
        if (!opt)
        {
            errMsg = std::string("channel(" + channel_id + ") is invalid");
            session->SendNok(PKT_SELECT_CHANNEL, errMsg);
        }
        else
        {
            ChannelInfo info = *opt;
            std::vector<std::string> channel_info;
            channel_info.push_back(info.ip);
            channel_info.push_back(std::to_string(info.port));
            session->Send(PKT_SELECT_CHANNEL, channel_info);
        }
    }
}
