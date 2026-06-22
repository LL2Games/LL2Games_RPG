#pragma once
#include "Packet.h"
#include "IPacketHandler.h"
#include "Player.h"
#include "MySqlConnectionPool.h"
#include <vector>
#include <string>

class ChannelInitHandler : public IPacketHandler
{
public:
    void Execute(PacketContext * ctx) override;
    void HandleChannelAuth(PacketContext *ctx);

};