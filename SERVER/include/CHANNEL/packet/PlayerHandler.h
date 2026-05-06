#pragma once
#include "common.h"
#include "Packet.h"
#include "IPacketHandler.h"
#include "Player.h"
#include "MySqlConnectionPool.h"


class PlayerHandler : public IPacketHandler
{
public:
    PlayerHandler(uint16_t type);
    void Execute(PacketContext * ctx) override;

    //stat
    void HandleStatView(PacketContext* ctx);
    void HandleStatUp(PacketContext* ctx);

    void MovePacket(PacketContext* ctx);
    void AttackPacket(PacketContext* ctx);
    void BasicAttackPacket(PacketContext* ctx);
    void OnDamagedPacket(PacketContext* ctx);
    void UseItemPacket(PacketContext* ctx);
  
private:
    uint16_t m_type;
};