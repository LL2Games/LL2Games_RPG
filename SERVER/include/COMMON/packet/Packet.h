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
class PlayerService;
class MapService;


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
    PKT_CHAT_INIT = 0x03,
    PKT_CHAT = 0x04,
    PKT_INIT_WORLD = 0x05,
    PKT_SELECT_CHARACTER = 0x06,
    PKT_SELECT_CHANNEL = 0x07,
    PKT_INIT_CHANNEL = 0x08,
    PKT_CHANNEL_AUTH = 0x09,
    PKT_ENTER_MAP = 0x0A,
    PKT_PLAYER_MOVE = 0x0B,
    PKT_PLAYER_ATTACK = 0x0C,
    PKT_PLAYER_SKILL = 0x0D,
    PKT_PLAYER_USE_ITEM = 0x0E,
};

struct ParsedPacket
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

    // Chat에서 사용
    std::vector<Client*>* clients = nullptr;
    std::function<void(const std::string&, const std::string&, int)>broadcast;

    //World
    WorldSession *world_session = nullptr;
    CharacterService *char_service = nullptr;
    ChannelManager *channel_manager = nullptr;

    //Channel에서 사용
    ChannelSession *channel_session = nullptr;
    PlayerService* player_service = nullptr;
    PlayerManager* player_manager =nullptr;
    MapService* map_service = nullptr;

};

typedef struct packet
{
    PacketHeader header{};
    std::vector<uint8_t> payload;
} Packet;