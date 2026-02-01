#include <vector>
#include <string>
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
