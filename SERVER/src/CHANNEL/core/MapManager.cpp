#include "CHANNEL/core/common.h"
#include "CHANNEL/core/MapManager.h"
#include "CHANNEL/core/PlayerManager.h"
#include <nlohmann/json.hpp>
#include <fstream>

#define MAP_PATH "./data/Maps/"


MapManager::MapManager()
{
}

MapManager::~MapManager()
{
}

void MapManager::Init()
{

}

void MapManager::Update()
{
    for(auto iter = m_maps.begin(); iter != m_maps.end(); ++iter)
    {
        if(iter->second != nullptr)
            iter->second->Update();
    }
}

int MapManager::CreateMap(int mapId)
{
    if(m_maps.find(mapId) != m_maps.end())  return -1;

    MapInitData mapData;

    std::string path = MAP_PATH + std::to_string(mapId) +".json";
    std::ifstream file(path);

    if(!file.is_open()) {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return -1;
    }

    // JSON 파일 파싱
    nlohmann::json j;
    file >> j;

	mapData.name = j["name"];
	mapData.mapID = j["mapId"];
	
	
    // Json 파일에서 몬스터 데이터 읽어오기
    LoadMonster(j, mapData.MonstersData);

    MapInstance* newMap = new MapInstance();

    if(newMap->Init(mapData) == 0)
    {
        m_maps[mapId]= newMap;
    }

    return 1;
}

void MapManager::LoadMonster(nlohmann::json& j, std::vector<MonsterSpawnData> MonstersData)
 {
    /*
    struct MonsterSpawnData {
    MonsterType type;
    bool isAlive;
    float timer;
    float respawnDelay;
};
    */
	
    auto MonsterCount = j["Monster"].size();

    MonstersData.resize(MonsterCount);
    // Json 파일에서 Monster 배열의 정보들을 반복문을 통해 설정
    for(const auto& Monster : j["monsters"]){
        MonsterSpawnData data;
	
        data.monsterId = Monster["monsterId"];
        data.spawnPos.xPos = Monster["x"];
        data.spawnPos.yPos = Monster["y"];
        data.ItemId = Monster["group"];
        MonstersData.push_back(data);
    }

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] MonsterLoad Success", __FUNCTION__, __LINE__);
 }