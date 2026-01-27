#pragma once
#include "CHANNEL/core/common.h"
#include "COMMON/CommonEnum.h"
#include "CHANNEL/core/MapInstance.h"
#include <nlohmann/json.hpp>
#include <queue>

class MapManager
{
public:
    MapManager();
    ~MapManager();

    void Init();
    void Update();
    MapInstance* GetOrCreate(int mapId);
    void LoadMonster(nlohmann::json& j, std::vector<MonsterSpawnData> MonstersData);
    void RemoveMap();
private:
    // key : map_id value : map
    std::unordered_map<int, MapInstance*> m_maps;
    std::queue<uint16_t> m_destroyQueue;

};
