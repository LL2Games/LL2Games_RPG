#include "CHANNEL/core/common.h"
#include "CHANNEL/core/MapManager.h"

MapManager::MapManager()
{
}

MapManager::~MapManager()
{
}

int MapManager::CreateMap(MapRegion mapType)
{
    //이미 맵이 만들어져 있는지 확인 후 만들어져 있으면 플레이어 이동
    if(m_maps.find(mapType) == m_maps.end())
    {

    }

    // MapData 읽어오기
    MapInitData data;

    MapInstance* newMap = new MapInstance();

    if(newMap->Init(data) == 0)
    {
        m_maps[mapType]= newMap;
    }


}