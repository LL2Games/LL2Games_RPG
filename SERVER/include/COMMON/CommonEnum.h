#include <vector>
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
    float respawnRemain;
    float respawnDelay;
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
    char name;
};


typedef struct MapInitData{
    MapRegion mapID;
    std::vector<MonsterSpawnData> MonstersData;

}mapData;
