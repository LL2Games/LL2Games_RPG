#pragma once
#include "IPacketHandler.h"

class ChatMsgHandler: public IPacketHandler{
public:
    void Execute(Client *client, const char *payload, const int len, const std::vector<Client*>& clients, std::function<void(const std::string&, const int)>(broadcast)) override;
};