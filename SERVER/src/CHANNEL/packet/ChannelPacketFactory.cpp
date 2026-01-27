#include "Packet.h"
#include "ChannelPacketFactory.h"
#include "PlayerHandler.h"
#include "MapHandler.h"

std::unique_ptr<IPacketHandler> ChannelPacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        case PKT_INIT_CHANNEL:
        case PKT_CHANNEL_AUTH:
            return std::make_unique<PlayerHandler>();
            break;
        case PKT_ENTER_MAP:
            return std::make_unique<MapHandler>();
            break;
        default:
            break;
    }
    return nullptr;
}