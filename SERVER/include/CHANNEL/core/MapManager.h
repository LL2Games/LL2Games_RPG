#pragma once
#include "CHANNEL/core/common.h"
#include "COMMON/CommonEnum.h"
#include "CHANNEL/core/MapInstance.h"
#include <nlohmann/json.hpp>

class MapManager
{
public:
    MapManager();
    ~MapManager();

    int CreateMap(int mapId);
    void LoadMonsters(nlohmann::json& j, std::vector<MonsterSpawnData> MonstersData);
private:
    // key : map_id value : map
    std::unordered_map<int, MapInstance*> m_maps;


};
