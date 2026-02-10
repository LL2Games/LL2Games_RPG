#include "PlayerHandler.h"
#include "CommonEnum.h"
#include "PacketParser.h"
#include "ChannelSession.h"
#include "PlayerManager.h"
#include "MapInstance.h"
#include "K_slog.h"
#include "PacketParser.h"



PlayerHandler::PlayerHandler(uint16_t type) : m_type(type)
{

}


void PlayerHandler::Execute(PacketContext * ctx)
{
     switch(ctx->pkt.type)
     {
          case PKT_PLAYER_MOVE:
               MovePacket(ctx);
               break;
          case PKT_PLAYER_ATTACK:
               AttackPacket(ctx);
               break;
          case PKT_PLAYER_ONDAMAGED:
               OnDamagedPacket(ctx);
               break;
          case PKT_PLAYER_USE_ITEM:
               UseItemPacket(ctx);
               break;
          case PKT_STAT_VIEW:
                HandleStatView(ctx);
                break;
          case PKT_STAT_UP:
                HandleStatUp(ctx);
                break;
          default :
               break;
     }
    
}

void PlayerHandler::MovePacket(PacketContext * ctx)
{
     ChannelSession *session = nullptr;
     Player* player = nullptr;
     MapInstance* map = nullptr;
     std::string errMsg;
     size_t offset = 0;
     int rc = EXIT_SUCCESS;
     float speed = 0;

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
        return;
    }

    map = player->GetCurrentMap();
     if(player == nullptr) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] current_map is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        return;
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

    // stoi 등 형식 변환은 위험 가능성이 높다
   
    PlayerPos.xPos = stof(playerXPos);
    PlayerPos.yPos = stof(playerYPos);
    speed = stof(str_speed);

 
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

void PlayerHandler::AttackPacket(PacketContext * ctx)
{   
     (void)ctx;
}

void PlayerHandler::OnDamagedPacket(PacketContext * ctx)
{
     (void)ctx;
}

void PlayerHandler::UseItemPacket(PacketContext * ctx)
{
     (void)ctx;
}
