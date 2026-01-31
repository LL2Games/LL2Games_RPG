#include "Packet.h"
#include "ChannelPacketFactory.h"
#include "PlayerHandler.h"

std::unique_ptr<IPacketHandler> ChannelPacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        case PKT_INIT_CHANNEL:
        case PKT_CHANNEL_AUTH:
        case PKT_STAT_VIEW:
        case PKT_STAT_UP:
            return std::make_unique<PlayerHandler>();
        default:
            break;
    }
    return nullptr;
}