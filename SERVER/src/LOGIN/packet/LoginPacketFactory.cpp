#include "Packet.h"
#include "LoginPacketFactory.h"
#include "LoginHandler.h"

std::unique_ptr<IPacketHandler> LoginPacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        case PKT_LOGIN: return std::make_unique<LoginHandler>();
    }

    return nullptr;
}