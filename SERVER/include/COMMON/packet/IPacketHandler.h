#pragma once
#include "Packet.h"
#include <vector>
#include <functional>

class IPacketHandler
{
public:
    virtual ~IPacketHandler() = default;

    virtual void Execute(PacketContext* ctx) = 0;
};