#pragma once
#include "IPacketHandler.h"

class ChatInitHandler: public IPacketHandler{
public:
    void Execute(Client *client, const char *payload, const int len, const std::vector<Client *> &, std::function<void(const std::string&, const std::string &, const int)>) override;
};