#include <vector>
#include <string>
#include <cstdint>
#include <optional>

#pragma once
enum class MapRegion{
    HENESYS,
    ELLINIA,
    PERION,
    KERNING_CITY
};

enum class MonsterType {
    SNAIL,
    ORANGE_MUSHROOM,
    SLIME,
    STUMP,
    PIG
};


struct Vec2{
    float xPos;
    float yPos;
};

struct MonsterSpawnData {
    MonsterType type;
    Vec2 spawnPos;
    int respawnDelay;
    int monsterId;
    int instanceId;
    int ItemId;
};

struct MonsterTemplate {
    int monsterId;
    int level;
    int hp;
    int attackDamage;
    float exp;
    float moveSpeed;
    std::string name;
};


typedef struct MapInitData{
    uint16_t mapID;
	std::string name;
    std::vector<MonsterSpawnData> MonstersData;

}mapData;


struct UseEffect
{
    int hp_restore = 0;
    int mp_restore = 0;
    int cooldown_ms = 0;
};

struct ItemInitData
{
    int item_id = 0;
    std::string name;
    std::string type;  // "consumable" / "equipment" /
    bool stackable = false;
    int max_stack = 1;
    int sell_price = 0;

    std::optional<UseEffect> use_effect; // consumable에만 존재
};