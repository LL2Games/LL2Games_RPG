#include <vector>
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

typedef struct SpawnInfo {
    MonsterType type;
    uint16_t spawnNum;
    float respawnDelay;
};

struct SpawnStatus {
    MonsterType type;
    bool isAlive;
    float timer;
    float respawnDelay;

    // 설정 데이터를 바탕으로 상태 데이터를 만드는 생성자
    SpawnStatus(const SpawnInfo& info) 
        : type(info.type), isAlive(false), timer(0.0f), respawnDelay(info.respawnDelay) {}
};

typedef struct MapInitData{
    MapRegion mapID;
    std::vector<SpawnInfo> spawns;

};
