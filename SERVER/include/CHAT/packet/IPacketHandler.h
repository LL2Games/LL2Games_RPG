#pragma once
#include "CHAT/core/Client.h"
#include <vector>
#include <functional>

class IPacketHandler
{
public:
    virtual ~IPacketHandler() = default;

    virtual void Execute(Client *client, const char *payload, const int len, const std::vector<Client*>& clients, std::function<void(const std::string&, const std::string&, const int)>(broadcast)) = 0;

    //virtual void Execute(Client *client, const char *payload, const int len) = 0;
};