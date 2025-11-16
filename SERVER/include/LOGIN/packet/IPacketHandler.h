#pragma once
#include <vector>
#include <string>
#include "core/Client.h"

// Command

class IPacketHandler
{
public:
    virtual ~IPacketHandler() = default;
    virtual void Execute(Client* client, const char* payload, const int len) = 0;
};