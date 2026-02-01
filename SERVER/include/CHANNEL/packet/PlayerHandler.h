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

    void Move(PacketContext * ctx);
    void Attack(PacketContext * ctx);
    void OnDamaged(PacketContext * ctx);
    void UseItem(PacketContext * ctx);
  
private:
    uint16_t m_type;
};