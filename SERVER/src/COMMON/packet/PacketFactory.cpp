#include "COMMON/packet/Packet.h"
#include "COMMON/packet/PacketFactory.h"
#include "CHANNEL/packet/PlayerHandler.h"

std::unique_ptr<IPacketHandler> PacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        case PKT_INIT_CHANNEL:
        case PKT_CHANNEL_AUTH:
            return std::make_unique<PlayerHandler>();
        default:
            break;
    }
    return nullptr;
}