#pragma once
#include <memory>
#include "Packet.h"
#include "IPacketHandler.h"
#include "IPacketFactory.h"

class ChatPacketFactory : public IPacketFactory
{
public:
    std::unique_ptr<IPacketHandler> Create(uint16_t type) override;
};