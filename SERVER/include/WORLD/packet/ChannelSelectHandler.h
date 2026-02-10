#pragma once
#include "Packet.h"
#include "IPacketHandler.h"

class ChannelSelectHandler : public IPacketHandler
{
public:
    void Execute(PacketContext* ctx) override;
};