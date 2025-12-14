#pragma once
#include "Packet.h"
#include "IPacketHandler.h"

class CharacterHandler : public IPacketHandler
{
public:
    void Execute(PacketContext* ctx) override;
};