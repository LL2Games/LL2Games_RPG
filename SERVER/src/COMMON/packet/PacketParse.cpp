#include "COMMON/packet/Packet.h"
#include "COMMON/packet/PacketParser.h"
#include "COMMON/packet/PacketFactory.h"
#include <cstring>
#include "K_slog.h"

std::optional<ParsePacket> PacketParser::Parse(std::vector<char>& buf)
{
    ParsePacket parsedPacket;
    if(buf.size() < sizeof(PacketHeader))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size() < sizeof(PacketHeader)", __FUNCTION__, __LINE__);
        return std::nullopt;
    }

    PacketHeader *hdr = reinterpret_cast<PacketHeader *>(buf.data());
    uint16_t pktLen = hdr->length;

    if(buf.size() < pktLen)
    {
         K_slog_trace(K_SLOG_ERROR, "[%s] buf.size small pktLen", __FUNCTION__);
        return std::nullopt;
    }

    uint16_t type = hdr->type;

    const char *payload = reinterpret_cast<const char*>(buf.data() + sizeof(PacketHeader));
    int payloadLen = pktLen - sizeof(PacketHeader);

    parsedPacket.type = type;
    parsedPacket.payload = std::string(payload, payloadLen);

    K_slog_trace(K_SLOG_TRACE, "[%s] type=%x", __FUNCTION__, type);

    buf.erase(buf.begin(), buf.begin() + pktLen);

    return parsedPacket;
}

std::string PacketParser::MakeBody(const std::vector<std::string>& datas)
{
    std::string body;

    for(const auto& data : datas)
    {
        if (!body.empty()) {
            body += "$";
        }
        body += data;
    }

    return body;
}

std::string PacketParser::MakePacket(uint16_t type, const std::string &body)
{
    PacketHeader hdr;
    std::string packet;

    hdr.type = type;
    hdr.length = sizeof(PacketHeader) + body.size();

    packet.append((char*)&hdr, sizeof(hdr));
    packet.append(body);
    
    return packet;
}