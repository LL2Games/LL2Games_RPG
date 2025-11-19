#pragma once
#include "CHAT/core/Client.h"
#include <functional>

class PacketParser {
public:
    static void Parse(Client* cli, const std::vector<Client*> &clients, std::function<void(const std::string&, const int)>broadcast);
};