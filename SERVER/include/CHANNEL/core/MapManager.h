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

    void Init();
    void Update();
    int CreateMap(int mapId);
    void LoadMaps(nlohmann::json& j, std::vector<MonsterSpawnData> MonstersData);
private:
    // key : map_id value : map
    std::unordered_map<int, MapInstance*> m_maps;
 

};
