#pragma once
#include "Packet.h"
#include "CHAT/core/Client.h"
#include <functional>
#include <cstdint>
#include <optional>

class PacketParser {
public:
    static std::optional<ParsedPacket> Parse(Client *cli);
    //static void Parse(Client *cli);
    static void Parse(Client* cli, const std::vector<Client*> &clients, std::function<void(const std::string&, const std::string&, const int)>broadcast);

    static std::string MakeChatBody(const std::string &nick, const std::string &msg);

    static std::string MakePacket(uint16_t type, const std::string& body);
};