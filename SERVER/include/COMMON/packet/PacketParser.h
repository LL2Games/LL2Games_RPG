#pragma once
#include "Packet.h"
#include <functional>
#include <cstdint>
#include <optional>
#include <vector>

class PacketParser {
public:
    static std::optional<ParsedPacket> Parse(std::vector<char>& buf);
    static bool ParseLengthPrefixedString( const char *payload, const size_t payload_len, size_t &offset, std::string &outValue, std::string &errMsg);
    static std::string MakeBody(const std::vector<std::string>& datas);
    static std::string MakePacket(uint16_t type, const std::string& body);

};