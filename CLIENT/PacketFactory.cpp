#include "pch.h"
#include "PacketFactory.h"

std::unique_ptr<IPacketHandler> PacketFactory::Create(uint16_t type)
{
	switch (type)
	{
	//case PKT_LOGIN: return std::make_unique<LoginHandler>();
	//case PKT_REGISTER:
	case PKT_CHAT: return std::make_unique<ChatHandler>();
	}
	return nullptr;
}
