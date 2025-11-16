#include "packet/PacketFactory.h"
#include "packet/Packet.h"
#include "packet/LoginHandler.h"

std::unique_ptr<IPacketHandler> PacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        case PKT_LOGIN: return std::make_unique<LoginHandler>();
        case PKT_REGISTER:
        default: 
            return nullptr;
    }
}