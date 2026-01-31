#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

class Client;

class RedisClient;
class MySqlconnectionPool;

class WorldServer;
class WorldSession;
class CharacterService;
class ChannelManager;

class ChannelServer;
class ChannelSession;
class PlayerManager;


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
    PKT_CHAT_INIT,
    PKT_CHAT,
    PKT_INIT_WORLD,
    PKT_SELECT_CHARACTER,
    PKT_SELECT_CHANNEL,
    PKT_INIT_CHANNEL,
    PKT_CHANNEL_AUTH,
    PKT_STAT_VIEW,
    PKT_STAT_UP,
};

struct ParsedPacket
{
    uint16_t type;
    std::string payload;
};

struct PacketContext
{
    uint16_t type;

    Client* client = nullptr;
    int fd = -1;
    char *payload = nullptr;
    int payload_len = 0;

    // Chat에서 사용
    std::vector<Client*>* clients = nullptr;
    std::function<void(const std::string&, const std::string&, int)>broadcast;

    //World
    WorldSession *world_session = nullptr;
    CharacterService *char_service = nullptr;
    ChannelManager *channel_manager = nullptr;

    //Channel에서 사용
    ChannelSession *channel_session = nullptr;
    PlayerManager* player_manager =nullptr;


};

typedef struct packet
{
    PacketHeader header{};
    std::vector<uint8_t> payload;
} Packet;