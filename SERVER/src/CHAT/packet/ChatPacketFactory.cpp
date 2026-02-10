#include "ChatPacketFactory.h"
#include "ChatInitHandler.h"
#include "ChatMsgHandler.h"

std::unique_ptr<IPacketHandler> ChatPacketFactory::Create(uint16_t type)
{
    switch(type)
    {
        case PKT_CHAT_INIT: return std::make_unique<ChatInitHandler>();
        case PKT_CHAT: return std::make_unique<ChatMsgHandler>();
    }

    return nullptr;
}