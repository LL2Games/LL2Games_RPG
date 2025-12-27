#pragma once
#include "Packet.h"
#include <functional>
#include <cstdint>
#include <optional>
#include <vector>

class PacketParser{
public:
    static std::optional<ParsePacket> Parse(std::vector<char>& buf);
    
    static std::string MakeBody(const std::vector<std::string>& datas);
    static std::string MakePacket(uint16_t type, const std::string& body);
};