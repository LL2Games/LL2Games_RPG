#include "Packet.h"
#include "PacketParser.h"
#include "PacketFactory.h"
#include <cstring>
#include "K_slog.h"

std::optional<ParsedPacket> PacketParser::Parse(std::vector<char>& buf)
{
    ParsedPacket parsedPacket;
    if (buf.size() < sizeof(PacketHeader))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size() < sizeof(PacketHeader)", __FUNCTION__, __LINE__);
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] sizeof(PacketHeader)[%d]", __FUNCTION__, __LINE__, sizeof(PacketHeader));
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size[%d]", __FUNCTION__, __LINE__, buf.size());
        return std::nullopt;
    }

    PacketHeader *hdr = reinterpret_cast<PacketHeader *>(buf.data());
    uint16_t pktLen = hdr->length;

    if (buf.size() < pktLen)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] buf.size small pktLen", __FUNCTION__);
        return std::nullopt;
    }
    uint16_t type = hdr->type;

    const char *payload = reinterpret_cast<const char *>(buf.data() + sizeof(PacketHeader));
    int payloadLen = pktLen - sizeof(PacketHeader);
    
    parsedPacket.type = type;
    parsedPacket.payload = std::string(payload, payloadLen);

    K_slog_trace(K_SLOG_TRACE, "[%s] type=%x", __FUNCTION__, type);
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] payload[%d]", __FUNCTION__, __LINE__, payloadLen);
    for (int i = 0; i < payloadLen; i++)
    {
        K_slog_trace(K_SLOG_TRACE, "[%x]", payload[i]);
    }

    buf.erase(buf.begin(), buf.begin() + pktLen);

    return parsedPacket;
}

std::string PacketParser::MakeBody(const std::vector<std::string>& datas)
{
    std::string body;

    for (auto& data : datas)
    {
        uint16_t dataLen = (uint16_t)data.size();
        body.append((char *)&dataLen, sizeof(dataLen));
        body.append(data);
    }

    return body;
}

std::string PacketParser::MakePacket(uint16_t type, const std::string &body)
{
    PacketHeader hdr;
    std::string packet;

    hdr.type = type;
    hdr.length = sizeof(PacketHeader) + body.size();

    packet.append((char *)&hdr, sizeof(hdr));
    packet.append(body);

    return packet;
}