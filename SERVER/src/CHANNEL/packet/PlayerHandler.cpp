#include "PlayerHandler.h"
#include "CommonEnum.h"
#include "PacketParser.h"
#include "ChannelSession.h"
#include "PlayerManager.h"
#include "MapInstance.h"





PlayerHandler::PlayerHandler(uint16_t type) : m_type(type)
{

}


void PlayerHandler::Execute(PacketContext * ctx)
{
     switch(ctx->type)
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
          default :
               break;
     }
    
}

