#pragma once
#include <cstdint>

#define BUFFER_SIZE 1024

#pragma pack(push, 1)
struct PacketHeader
{
    uint16_t length; // 전체 패킷 길이
    uint16_t type;   // 패킷 타입
};
#pragma pack(pop)

// 패킷타입 정의
enum PACKET_TYPE : uint16_t {
    PKT_LOGIN = 0x01,
    PKT_REGISTER = 0x02,
    PKT_CHAT = 0x03,
};