#include "common.h"
#include "Packet.h"
#include "IPacketHandler.h"
#include "Player.h"
#include "MySqlConnectionPool.h"
#include <vector>
#include <string>

class PlayerHandler : public IPacketHandler
{
public:
    PlayerHandler(uint16_t type);
    void Execute(PacketContext * ctx) override;

    void MovePacket(PacketContext * ctx);
    void AttackPacket(PacketContext * ctx);
    void OnDamagedPacket(PacketContext * ctx);
    void UseItemPacket(PacketContext * ctx);
  
private:
    uint16_t m_type;
};