#pragma once
#include "common.h"
#include "IPacketHandler.h"

class InventoryPacketHandler : public IPacketHandler
{
public:
    void Execute(PacketContext * ctx) override;

};