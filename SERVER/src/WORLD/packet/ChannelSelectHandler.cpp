#include "packet/ChannelSelectHandler.h"
#include "WorldSession.h"
#include "ChannelManager.h"
#include "K_slog.h"
#include "PacketParser.h"


void ChannelSelectHandler::Execute(PacketContext* ctx)
{
    WorldSession *session = nullptr;
    ChannelManager *channel_manager = nullptr;
    int rc = EXIT_SUCCESS;
    std::string errMsg;
    size_t offset = 0;
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
    
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            channel_id,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }

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
            session->SendOk(PKT_SELECT_CHANNEL, channel_info);
        }
    }
}
