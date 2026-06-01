#pragma once
#include "common.h"
#include "Packet.h"
#include "IPacketHandler.h"
#include "Player.h"
#include "MySqlConnectionPool.h"


class QuickSlotPacketHandler : public IPacketHandler
{
public:
    void Execute(PacketContext * ctx) override;
    void HandleSetQuickSlot(PacketContext* ctx);
private:
};
