#include "Packet.h"
#include "ChannelPacketFactory.h"
#include "ChannelInitHandler.h"
#include "MapHandler.h"
#include "PlayerHandler.h"
#include "InventoryPacketHandler.h"
#include "K_slog.h"
std::unique_ptr<IPacketHandler> ChannelPacketFactory::Create(uint16_t type)
{
    
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
        case PKT_PLAYER_BASIC_ATTACK:
        case PKT_PLAYER_ONDAMAGED:
        case PKT_PLAYER_USE_ITEM:
        case PKT_STAT_VIEW:
        case PKT_STAT_UP:
        case PKT_TRADE_REQUEST:
        case PKT_TRADE_ACCEPT:
        case PKT_TRADE_READY:
        case PKT_TRADE_CANCEL:
            return std::make_unique<PlayerHandler>(type);
        case PKT_INVENTORY_ITEM_MOVE:
            return std::make_unique<InventoryPacketHandler>();
            break;
        default:
            break;
    }
    return nullptr;
}