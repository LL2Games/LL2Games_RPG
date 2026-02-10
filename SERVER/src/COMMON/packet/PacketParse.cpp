#include "Packet.h"
#include "PacketParser.h"
#include "PacketFactory.h"
#include <cstring>
#include "K_slog.h"

std::optional<ParsedPacket> PacketParser::Parse(std::vector<char>& buf)
{
    ParsedPacket parsedPacket;
    if(buf.size() < sizeof(PacketHeader))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size() < sizeof(PacketHeader)", __FUNCTION__, __LINE__);
        return std::nullopt;
    }

    PacketHeader *hdr = reinterpret_cast<PacketHeader *>(buf.data());
    uint16_t pktLen = hdr->length;  // 네트워크 바이트 순서 변환 추가

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

#if 1 /* payload 확인 로그*/
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] payloadLen[%d]", __FUNCTION__, __LINE__, payloadLen);
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] LJH TEST", __FUNCTION__ , __LINE__);
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] type=%x", __FUNCTION__, __LINE__, type);
#endif

    buf.erase(buf.begin(), buf.begin() + pktLen);

    K_slog_trace(K_SLOG_TRACE, " [%s][%d] LJH TEST", __FUNCTION__ , __LINE__);
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