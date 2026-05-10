#pragma once
#include "common.h"
#include "Player.h"
// 아이템 드롭 관련 구조체
enum class DropType
{
    Gold,
    Item,

};

struct DropEntry
{
    DropType type;
    int itemId = 0;
    int weight = 0;
    int minCount = 1;
    int maxCount = 1;
    std::string tag;
};

struct DropGroup
{
    std::string groupId;
    int minDrop = 0;
    int maxDrop = 0;
    bool allowDuplicate = false;
    std::vector<DropEntry> entries;
};

// 서버내에서 맵에 생성된 아이템 관리용
struct DropItems
{
    int64_t dropId = 0;
    DropType type = DropType::Item;
    int itemId = 0;
    int count = 0;

    Vec2 pos;

    int64_t expireTimeMs =0;
    int64_t ownerExpireTimeMs = 0;

    Player* owner;

};

// 클라이언트한테 드롭 아이템에 관한 정보 전달용
struct DropSpawnInfo
{
    int64_t dropId = 0;
    DropType type;
    int itemId = 0;
    int count = 0;
    float xPos = 0.0f;
    float yPos = 0.0f;
};

struct DropResult
{
    DropType type;
    int itemId;
    int count;
};