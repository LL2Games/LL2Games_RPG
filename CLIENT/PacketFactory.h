#pragma once
#include "Packet.h"
#include <memory>

class IPacketHandler;

class PacketFactory
{
public:
	static std::unique_ptr<IPacketHandler> Create(uint16_t type);
};

