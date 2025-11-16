#pragma once
#include "IPacketHandler.h"

// Command

class LoginHandler : public IPacketHandler{
public:
    void Execute(Client* client, const char* payload, const int len) override;
};