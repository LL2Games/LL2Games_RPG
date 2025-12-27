#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

class Client;
class ChannelServer;
class ChannelSession;
class MySqlconnectionPool;
class PlayerManager;



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
    PKT_CHAT_INIT,
    PKT_CHAT,
    PKT_INIT_WORLD,
    PKT_SELECT_CHARACTER,
    PKT_SELECT_CHANNEL,
    PKT_INIT_CHANNEL,
    PKT_CHANNEL_AUTH
};

struct ParsePacket
{
    uint16_t type;
    std::string payload;
};

struct PacketContext
{
    Client* client = nullptr;
    int fd = -1;
    char *payload = nullptr;
    int payload_len = 0;


    //Channel에서 사용
    ChannelSession *session = nullptr;
    PlayerManager* player_manager =nullptr;


    // Chat에서 사용
    std::vector<Client*>* clients = nullptr;
    std::function<void(const std::string&, const std::string&, int)>broadcast;
};

typedef struct packet
{
    PacketHeader header{};
    std::vector<uint8_t> payload;
} Packet;