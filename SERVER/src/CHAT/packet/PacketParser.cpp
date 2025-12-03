#include "CHAT/packet/PacketParser.h"
#include "CHAT/packet/PacketFactory.h"
#include "Packet.h"
#include <cstring>
#include "K_slog.h"

std::optional<ParsedPacket> PacketParser::Parse(Client *cli)
{
    auto &buf = cli->m_recvBuffer;
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
    K_slog_trace(K_SLOG_TRACE, "[%s] type=%x", __FUNCTION__, type);

    parsedPacket.type = type;
    parsedPacket.payload = std::string(payload, payloadLen);
    buf.erase(buf.begin(), buf.begin() + pktLen);

    return parsedPacket;
}

// void PacketParser::Parse(Client *cli)
// {
//     auto &buf = cli->m_recvBuffer;

//     while (buf.size())
//     {
//         if (buf.size() < sizeof(PacketHeader))
//         {
//             K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size() < sizeof(PacketHeader)", __FUNCTION__, __LINE__);
//             K_slog_trace(K_SLOG_ERROR, "[%s][%d] sizeof(PacketHeader)[%d]", __FUNCTION__, __LINE__, sizeof(PacketHeader));
//             K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size[%d]", __FUNCTION__, __LINE__, buf.size());
//             return;
//         }

//         PacketHeader *hdr = reinterpret_cast<PacketHeader *>(buf.data());
//         uint16_t pktLen = hdr->length;

//         if (buf.size() < pktLen)
//         {
//             K_slog_trace(K_SLOG_ERROR, "[%s] buf.size small pktLen", __FUNCTION__);
//             return;
//         }
//         uint16_t type = hdr->type;

//         const char *payload = reinterpret_cast<const char *>(buf.data() + sizeof(PacketHeader));
//         int payloadLen = pktLen - sizeof(PacketHeader);
//         K_slog_trace(K_SLOG_TRACE, "[%s] type=%x", __FUNCTION__, type);

//         auto handler = PacketFactory::Create(type);
//         if (handler)
//             handler->Execute(cli, payload, payloadLen);

//         buf.erase(buf.begin(), buf.begin() + pktLen);
//     }
// }

void PacketParser::Parse(Client *cli, const std::vector<Client *> &clients, std::function<void(const std::string &, const std::string &, const int)> broadcast)
{
    auto &buf = cli->m_recvBuffer;

    while (buf.size())
    {
        // K_slog_trace(K_SLOG_TRACE, "[%s]buf size[%d]", __FUNCTION__, buf.size());
        // for (int i = 0; i < (int)buf.size(); i++)
        // {
        //     K_slog_trace(K_SLOG_TRACE, "[%s]%x ", __FUNCTION__, buf[i]);
        // }
        if (buf.size() < sizeof(PacketHeader))
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size() < sizeof(PacketHeader)", __FUNCTION__, __LINE__);
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] sizeof(PacketHeader)[%d]", __FUNCTION__, __LINE__, sizeof(PacketHeader));
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size[%d]", __FUNCTION__, __LINE__, buf.size());
            return;
        }

        PacketHeader *hdr = reinterpret_cast<PacketHeader *>(buf.data());
        uint16_t pktLen = hdr->length;

        if (buf.size() < pktLen)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s] buf.size small pktLen", __FUNCTION__);
            return;
        }
        uint16_t type = hdr->type;

        const char *payload = reinterpret_cast<const char *>(buf.data() + sizeof(PacketHeader));
        int payloadLen = pktLen - sizeof(PacketHeader);
        K_slog_trace(K_SLOG_TRACE, "[%s] type=%x", __FUNCTION__, type);

        auto handler = PacketFactory::Create(type);
        if (handler)
            handler->Execute(cli, payload, payloadLen, clients, broadcast);

        buf.erase(buf.begin(), buf.begin() + pktLen);
    }
}

std::string PacketParser::MakeChatBody(const std::string &nick, const std::string &msg)
{
    std::string body;

    // 닉네임
    uint8_t nickLen = (uint8_t)nick.size();
    body.append((char *)&nickLen, sizeof(nickLen));
    body.append(nick);

    // 메세지
    uint16_t msgLen = (uint16_t)msg.size();
    body.append((char *)&msgLen, sizeof(msgLen));
    body.append(msg);

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