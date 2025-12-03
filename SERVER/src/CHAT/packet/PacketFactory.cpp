#include "CHAT/packet/PacketFactory.h"
#include "CHAT/packet/ChatInitHandler.h"
#include "CHAT/packet/ChatMsgHandler.h"

std::unique_ptr<IPacketHandler> PacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        case PKT_CHAT_INIT: return std::make_unique<ChatInitHandler>();
        case PKT_CHAT: return std::make_unique<ChatMsgHandler>();
    }

    return nullptr;
}