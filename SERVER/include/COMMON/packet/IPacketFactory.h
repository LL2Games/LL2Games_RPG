#pragma once
#include <memory>
#include "Packet.h"
#include "IPacketHandler.h"

class IPacketFactory
{
public:
    virtual ~IPacketFactory() = default;

    virtual std::unique_ptr<IPacketHandler> Create(uint16_t type) = 0;
};