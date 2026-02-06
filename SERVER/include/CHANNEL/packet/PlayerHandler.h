#pragma once
#include "common.h"
#include "Packet.h"
#include "IPacketHandler.h"
#include "Player.h"
#include "MySqlConnectionPool.h"

void MovePacket(PacketContext * ctx);
void AttackPacket(PacketContext * ctx);
void OnDamagedPacket(PacketContext * ctx);
void UseItemPacket(PacketContext * ctx);

class PlayerHandler : public IPacketHandler
{
public:
    PlayerHandler(uint16_t type);
    void Execute(PacketContext * ctx) override;

private:
    uint16_t m_type;
};