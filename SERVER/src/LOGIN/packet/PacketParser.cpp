#include "packet/PacketParser.h"
#include "packet/Packet.h"
#include "packet/PacketFactory.h"
#include <cstring>

void PacketParser::Parse(Client* cli)
{
    auto& buf = cli->m_recvBuffer;

    while (true)
    {
        // 최소 헤더 길이
        if(buf.size() < sizeof(PacketHeader))
            return;
        
        PacketHeader *header = (PacketHeader*)buf.data();
        uint16_t pktLen = header->length;

        // 전체 패킷이 아직 도착 안함
        if (buf.size() < pktLen)
            return;
        
        // 패킷 완성됨 -> 처리
        uint16_t type = header->type;

        // payload 시작점
        const char *payload = buf.data() + sizeof(PacketHeader);
        int payloadLen = pktLen - sizeof(PacketHeader);

        // 핸들러 생성
        auto handler = PacketFactory::Create(type);
        if (handler)
            handler->Execute(cli, payload, payloadLen);
        
        // 사용한 만큼 버퍼 삭제
        buf.erase(buf.begin(), buf.begin() + pktLen);
    }
}
