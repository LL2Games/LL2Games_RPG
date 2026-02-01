#include "Packet.h"
#include "ChannelPacketFactory.h"
#include "ChannelInitHandler.h"
#include "MapHandler.h"
#include "PlayerHandler.h"
#include "K_slog.h"
std::unique_ptr<IPacketHandler> ChannelPacketFactory::Create(uint16_t type)
{
    K_slog_trace(K_SLOG_DEBUG, "[%d][%s]type=%d", __LINE__, __FUNCTION__, type);
    switch(type)
    {
        case PKT_INIT_CHANNEL:
        case PKT_CHANNEL_AUTH:
            return std::make_unique<ChannelInitHandler>();
            break;
        case PKT_ENTER_MAP:
            return std::make_unique<MapHandler>();
            break;
        case PKT_PLAYER_MOVE:
        case PKT_PLAYER_ATTACK:
        case PKT_PLAYER_SKILL:
        case PKT_PLAYER_USE_ITEM:
            return std::make_unique<PlayerHandler>(type);
        default:
            break;
    }
    return nullptr;
}