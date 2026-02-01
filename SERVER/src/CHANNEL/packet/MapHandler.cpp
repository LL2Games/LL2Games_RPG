#include "common.h"
#include "MapHandler.h"
#include "ChannelSession.h"
#include "PacketParser.h"
#include "K_slog.h"
#include <sstream>
#include <stdexcept>


void MapHandler::Execute(PacketContext * ctx)
{
    ChannelSession *session = nullptr;
    int rc = EXIT_SUCCESS;
    std::string errMsg;
    size_t offset = 0;
    int mapId =0;
    int playerid =0;

    std::string player_id;
    std::string map_id;


    if(ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ctx is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }

    session = ctx->channel_session;
    if(session == nullptr)
    {
          K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] session is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    
   // 받은 정보에서 playerID 추출 
    if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        player_id,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
    // 받은 정보에서 mapID 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        map_id,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    playerid = stoi(player_id);
    mapId = stoi(map_id);

    if(!ctx->map_service)
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] HandlePortalUse fail", __FILE__, __FUNCTION__, __LINE__);
        errMsg = "map_service is NULL";
        goto err;
    }
    rc = ctx->map_service->HandlePortalUse(playerid, mapId);

    if(rc != EXIT_SUCCESS)
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] HandlePortalUse fail", __FILE__, __FUNCTION__, __LINE__);
        errMsg = "HandlePortalUse Failed";
        goto err;
    }
   
err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_ENTER_MAP, errMsg);
    } else {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] MAP HANDLER END", __FILE__, __FUNCTION__, __LINE__);
        session->SendOk(PKT_ENTER_MAP);
    }
}

