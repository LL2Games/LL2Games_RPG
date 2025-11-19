#include "CHAT/packet/PacketParser.h"
#include "CHAT/packet/PacketFactory.h"
#include "CHAT/packet/Packet.h"
#include <cstring>

void PacketParser::Parse(Client* cli, const std::vector<Client*> &clients, std::function<void(const std::string&, const int)>broadcast)
{
    auto &buf = cli->m_recvBuffer;

    while (true)
    {
        if (buf.size() < sizeof(PacketHeader))
            return;

        PacketHeader *hdr = reinterpret_cast<PacketHeader *>(buf.data());
        uint16_t pktLen = hdr->length;

        if (buf.size() < pktLen)
            return;
        
        uint16_t type = hdr->type;

        const char *payload = reinterpret_cast<const char *>(buf.data() + sizeof(PacketHeader));
        int payloadLen = pktLen - sizeof(PacketHeader);

        auto handler = PacketFactory::Create(type);
        if (handler)
            handler->Execute(cli, payload, payloadLen, clients, broadcast);

        buf.erase(buf.begin(), buf.begin() + pktLen);
    }
}