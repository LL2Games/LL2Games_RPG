#pragma once
#include <memory>
#include "IPacketHandler.h"
#include "Packet.h"

class PacketFactory{
public:
    static std::unique_ptr<IPacketHandler> Create(uint16_t type);
};