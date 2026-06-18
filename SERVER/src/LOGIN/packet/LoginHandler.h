#pragma once
#include "IPacketHandler.h"

// Command
class LoginHandler : public IPacketHandler{
public:
    void Execute(PacketContext* ctx) override;
};