#pragma once
#include "CHANNEL/core/common.h"
#include "COMMON/CommonEnum.h"
#include "CHANNEL/core/MapInstance.h"

class MapManager
{
public:
    MapManager();
    ~MapManager();

    int CreateMap(MapRegion mapType);
private:
    // key : map_id value : map
    std::unordered_map<MapRegion, MapInstance*> m_maps;


};
