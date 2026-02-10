#include "Packet.h"
#include "PacketParser.h"
#include <cstring>
#include <type_traits>
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

bool PacketParser::ParseLengthPrefixedString(
    const char *payload,
    const size_t payload_len,
    size_t &offset,
    std::string &outValue,
    std::string &errMsg)
{
    if (payload == nullptr || payload_len == 0)
    {
        errMsg = "payload empty";
        return false;
    }

    if (offset >= payload_len)
    {
        errMsg = "offset overflow";
        return false;
    }

    // 1. length (1 byte)
    uint8_t value_len = static_cast<uint8_t>(payload[offset]);
    offset += 1;

    // 2. reserved byte skip
    if (offset < payload_len && payload[offset] == 0x00)
    {
        offset += 1;
    }

    // 3. bounds check
    if (offset + value_len > payload_len)
    {
        errMsg = "payload length overflow";
        return false;
    }

    // 4. extract value
    outValue.assign(payload + offset, value_len);
    offset += value_len;

    return true;
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


/*
template<typename T>
bool PacketParser::ParseLengthPrefixedString(
    const char *payload,
    const size_t payload_len,
    size_t &offset,
    T &outValue,
    std::string &errMsg)
{
    if (payload == nullptr || payload_len == 0)
    {
        errMsg = "payload empty";
        return false;
    }

    if (offset >= payload_len)
    {
        errMsg = "offset overflow";
        return false;
    }

    // 1. length (1 byte)
    uint8_t value_len = static_cast<uint8_t>(payload[offset]);
    offset += 1;

    // 2. reserved byte skip
    if (offset < payload_len && payload[offset] == 0x00)
    {
        offset += 1;
    }

    // 3. bounds check
    if (offset + value_len > payload_len)
    {
        errMsg = "payload length overflow";
        return false;
    }

    std::string_view str(payload + offset, value_len);
    offset += value_len;

    // 4. extract value
    if constexpr(std::is_same<T,std::string>)
    {
        outValue = std::string(str);
    }
    else if constexpr(std::is_same<T, int>)
    {
        try{
            outValue = std::stoi(std::string(str));
        }catch(...) {
            errMsg = "invalid int";
            return false;
        }
    }
    else if constexpr(std::is_same<T,float>)
    {
        try
        {
             outValue = std::stof(std::string(str));
        }
        catch(...)
        {
            errMsg = "invalid float";
            return false;
        }
        
    }
    
    return true;
}
*/