#include "common.h"
#include "PacketParser.h"
#include "ChannelSession.h"
#include "PlayerManager.h"
#include "MapInstance.h"
#include "utility.h"


void MovePacket(PacketContext * ctx)
{
    ChannelSession *session = nullptr;
    Player* player = nullptr;
    MapInstance* map = nullptr;

    size_t offset = 0;
    int rc = EXIT_SUCCESS;
    float speed = 0;

    std::string errMsg;
    std::string playerXPos;
    std::string playerYPos;
    std::string str_speed;
    Vec2 PlayerPos = {0,0};
     
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

    player = session->GetPlayer();
    if(player == nullptr) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] Player is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]Player is nullptr";
        goto err;
    }

    map = player->GetCurrentMap();
     if(map == nullptr) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] current_map is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]current_map is nullptr";
        goto err;
    }
    // 받은 정보에서 Xpos 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        playerXPos,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     // 받은 정보에서 Ypos 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        playerYPos,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     // 받은 정보에서 speed 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_speed,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    // stoi 등 형식 변환은 위험 가능성이 높다 따라서 utiliy 파일에서 int와 float 형식으로 변환하는 함수를 통해서 안전하게 변환
    if(!utility::StringToFloat(playerXPos, PlayerPos.xPos))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] playerXPos String to Float fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!utility::StringToFloat(playerYPos,  PlayerPos.yPos))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] playerYPos String to Float fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!utility::StringToFloat(str_speed,  speed))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] speed String to Float fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
   
    // 플레이어가 속한 맵의 모든 플레이어한테 변경 정보 전달
    map->HandleMove(player, PlayerPos, speed);
   
err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_ENTER_MAP, errMsg);
    } else {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] MAP HANDLER END", __FILE__, __FUNCTION__, __LINE__);
        session->SendOk(PKT_ENTER_MAP);
    }
}