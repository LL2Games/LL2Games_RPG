#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct PacketHeader
{
    uint16_t length; // 전체 패킷 길이
    uint16_t type;   // 패킷 타입
};
#pragma pack(pop)

// 패킷타입 정의
enum PACKET_TYPE : uint16_t {
    PKT_CHAT_MSG = 100,
};