#pragma once
#include "CHAT/core/Client.h"
#include <functional>
#include <cstdint>

class PacketParser {
public:
    static void Parse(Client* cli, const std::vector<Client*> &clients, std::function<void(const std::string&, const std::string&, const int)>broadcast);

    static std::string MakeChatBody(const std::string &nick, const std::string &msg);

    static std::string MakePacket(uint16_t type, const std::string& body);
};