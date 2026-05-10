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
class StatService;
class ItemManager;
class ItemService;
class CombatService;

class CommandDispatcher;


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
     // 0x0001 ~ 0x001F : 로그인 / 월드
    PKT_LOGIN               = 0x0001,
    PKT_REGISTER            = 0x0002,
    PKT_CHAT_INIT           = 0x0003,
    PKT_CHAT                = 0x0004,
    PKT_INIT_WORLD          = 0x0005,
    PKT_SELECT_CHARACTER    = 0x0006,
    PKT_SELECT_CHANNEL      = 0x0007,
    PKT_INIT_CHANNEL        = 0x0008,
    PKT_CHANNEL_AUTH        = 0x0009,
    PKT_ENTER_MAP           = 0x000A,

    // 0x0020 ~ 0x003F : 플레이어
    PKT_PLAYER_MOVE         = 0x0020,
    PKT_PLAYER_ATTACK       = 0x0021,
    PKT_PLAYER_ONDAMAGED    = 0x0022,
    PKT_PLAYER_USE_ITEM     = 0x0023,
    PKT_PLAYER_INFO         = 0x0024,
    PKT_PLAYER_STAT         = 0x0025,
    PKT_PLAYER_SKILLLIST    = 0x0026,
    PKT_PLAYER_BASIC_ATTACK = 0x0027,

    // 0x0040 ~ 0x005F : 몬스터
    PKT_MONSTER_MOVE        = 0x0040,
    PKT_MONSTER_ONDAMAGED   = 0x0041,
    PKT_MONSTER_SNAPSHOT    = 0x0042,

    // 0x0060 ~ 0x007F : 드롭
    PKT_DROPITEMS           = 0x0060,
    PKT_REMOVEITEMS         = 0x0061,

    // 0x0080 ~ 0x009F : 인벤토리
    PKT_INVENTORY_META_INFO = 0x0080,
    PKT_INVENTORY_ITEM_INFO = 0x0081,

    // 0x1000 ~ : 테스트 / UI / 특수
    PKT_STAT_VIEW           = 0x1000,
    PKT_STAT_UP             = 0x1001,
    PKT_QUICKSLOT_LIST      = 0x1002,
};

struct ParsedPacket
{
    uint16_t type;
    std::string payload;
};

struct PacketContext
{
    Client* client = nullptr;
    uint16_t type;
    int fd = -1;
    char *payload = nullptr;
    int payload_len = 0;

    // Chat에서 사용
    std::vector<Client*>* clients = nullptr;
    std::function<void(const std::string&, const std::string&, int)>broadcast;
    CommandDispatcher* dispatcher = nullptr;

    //World
    WorldSession *world_session = nullptr;
    CharacterService *char_service = nullptr;
    ChannelManager *channel_manager = nullptr;

    //Channel에서 사용
    ChannelSession *channel_session = nullptr;
    PlayerService* player_service = nullptr;
    PlayerManager* player_manager =nullptr;
    MapService* map_service = nullptr;
    StatService* stat_service = nullptr;
    ItemManager* item_manager =nullptr;
    ItemService* item_service = nullptr;
    CombatService* combat_service = nullptr;
};

typedef struct packet
{
    PacketHeader header{};
    std::vector<uint8_t> payload;
} Packet;