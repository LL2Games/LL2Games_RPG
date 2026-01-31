#include "PlayerHandler.h"
#include "ChannelSession.h"
#include "Player.h"
#include "PacketParser.h"

void PlayerHandler::HandleStatView(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    ChannelSession *session = nullptr;
    Player *player = nullptr;
    size_t char_id;
    std::string errMsg;
    // size_t offset = 0;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->channel_session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    char_id = player->GetId();
    if (char_id == 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] char_id is invalid\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]char_id is invalid";
        goto err;
    }
    
    //input 필요없을듯.
    // if (!PacketParser::ParseLengthPrefixedString(
    //         ctx->payload,
    //         ctx->payload_len,
    //         offset,
    //         channel_id,
    //         errMsg))
    // {
    //     rc = EXIT_FAILURE;
    //     K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
    //     goto err;
    // }

err:
    if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_STAT_VIEW, errMsg);
    }
    else
    {
        session->SendOk(PKT_STAT_VIEW);
        // auto opt = channel_manager->SelectChannel(channel_id);
        // if (!opt)
        // {
        //     errMsg = std::string("channel(" + channel_id + ") is invalid");
        //     session->SendNok(PKT_SELECT_CHANNEL, errMsg);
        // }
        // else
        // {
        //     ChannelInfo info = *opt;
        //     std::vector<std::string> channel_info;
        //     channel_info.push_back(info.ip);
        //     channel_info.push_back(std::to_string(info.port));
        //     session->SendOk(PKT_SELECT_CHANNEL, channel_info);
        // }
    }
}

void PlayerHandler::HandleStatUp(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    ChannelSession *session = nullptr;
    Player *player = nullptr;
    size_t char_id;
    std::string errMsg;
    size_t offset = 0;
    std::string stat;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->channel_session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    char_id = player->GetId();
    if (char_id == 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] char_id is invalid\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]char_id is invalid";
        goto err;
    }
    
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            stat,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }

err:
    if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_STAT_UP, errMsg);
    }
    else
    {
        session->SendOk(PKT_STAT_UP);
        // auto opt = channel_manager->SelectChannel(channel_id);
        // if (!opt)
        // {
        //     errMsg = std::string("channel(" + channel_id + ") is invalid");
        //     session->SendNok(PKT_SELECT_CHANNEL, errMsg);
        // }
        // else
        // {
        //     ChannelInfo info = *opt;
        //     std::vector<std::string> channel_info;
        //     channel_info.push_back(info.ip);
        //     channel_info.push_back(std::to_string(info.port));
        //     session->SendOk(PKT_SELECT_CHANNEL, channel_info);
        // }
    }
}