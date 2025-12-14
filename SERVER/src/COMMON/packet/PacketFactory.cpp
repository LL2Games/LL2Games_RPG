#include "Packet.h"
#include "PacketFactory.h"
#include "../WORLD/packet/WorldInitHandler.h"
#include "../WORLD/packet/CharacterHandler.h"

std::unique_ptr<IPacketHandler> PacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        /*case PKT_CHAT_INIT: return std::make_unique<ChatInitHandler>();
        case PKT_CHAT: return std::make_unique<ChatMsgHandler>();
        case PKT_LOGIN: return std::make_unique<LoginHandler>();
        */
        case PKT_INIT_WORLD: return std::make_unique<WorldInitHandler>();
        case PKT_SELECT_CHARACTER: return std::make_unique<CharacterHandler>();
    }

    return nullptr;
}