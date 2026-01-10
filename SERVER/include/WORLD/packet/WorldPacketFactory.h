#pragma once
#include <memory>
#include "Packet.h"
#include "IPacketHandler.h"
#include "IPacketFactory.h"

class WorldPacketFactory : public IPacketFactory
{
public:
    std::unique_ptr<IPacketHandler> Create(uint16_t type) override;
};