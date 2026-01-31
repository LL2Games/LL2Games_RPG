#pragma once
#include "COMMON/packet/Packet.h"
#include "COMMON/packet/IPacketHandler.h"
#include "CHANNEL/core/Player.h"
#include "CHANNEL/db/MySqlConnectionPool.h"
#include <vector>
#include <string>

class PlayerHandler : public IPacketHandler
{
public:
    void Execute(PacketContext * ctx) override;
    
    
    void HandleChannelAuth(PacketContext *ctx);
    void HandleStatView(PacketContext* ctx);
    void HandleStatUp(PacketContext* ctx);
};