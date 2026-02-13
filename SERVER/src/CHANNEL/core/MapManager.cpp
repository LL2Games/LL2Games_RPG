#include "common.h"
#include "MapManager.h"
#include "PlayerManager.h"

#define MAP_PATH "../src/CHANNEL/data/Maps/"
namespace fs = std::filesystem;

MapManager::MapManager()
{
}

MapManager::~MapManager()
{
}

void MapManager::Init()
{
    // 서버 구동 시 Map 데이터 전부 읽어서 미리 저장
    PreLoadAll();
}

void MapManager::Update()
{
    for(auto iter = m_maps.begin(); iter != m_maps.end(); ++iter)
    {
        if(iter->second != nullptr)
            iter->second->Update();
    }

    RemoveMap();
}

MapInstance* MapManager::GetOrCreate(int mapId)
{
    auto it = m_maps.find(mapId);

    if(it != m_maps.end())  
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] alreay Map is Maked", __FUNCTION__, __LINE__);
        return it->second;
    }

    K_slog_trace(K_SLOG_ERROR, "[%s][%d] MAPINIT START", __FUNCTION__, __LINE__);

    MapInitData mapData;

    auto itInit = m_maps_initData.find(mapId);
    if (itInit != m_maps_initData.end())
    {
        mapData = itInit->second; 
    }
    else
    {
        
        if (!LoadJsonFile(mapId, mapData))
            return nullptr;
    }

    MapInstance* newMap = new MapInstance();

    if(newMap->Init(mapData) == 1)
    {
        m_maps[mapId]= newMap;
    }else {
        delete newMap;
        return nullptr;
    }

    // 맵 생성 후 삭제 예약을 걸어둔다. 맵에 플레이어가 없는 경우에 일정 시간이 지나면 맵을 삭제할 수 있도록 설정
    newMap->SetDestroyCallback([this](int id){
        m_destroyQueue.push(id);
    });

    return newMap;
}

bool MapManager::PreLoadAll()
{
    for(const auto& entry : fs::directory_iterator(MAP_PATH))
    {
        if(!entry.is_regular_file()) continue;
        if(entry.path().extension() != ".json") continue;

        /*
            // entry.path() -> 파일 경로
            // entry.path().filename() -> 파일명
            // entry.path().stem() -> 확장자 제거한 파일명(예: "1001")
        
        */
        int map_id = std::stoi(entry.path().stem().string());

        auto it = m_maps_initData.find(map_id);
        if(it != m_maps_initData.end()) continue;

        MapInitData mapData;

        bool is_Load = LoadJsonFile(map_id, mapData);
        if(!is_Load) return false;

        m_maps_initData.emplace(map_id, std::move(mapData));
    }

    return true;
}

bool MapManager::LoadJsonFile(int mapId, MapInitData& mapData)
{

    std::string path = MAP_PATH + std::to_string(mapId) +".json";
    std::ifstream file(path);

    if(!file.is_open()) {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return false;
    }

    // JSON 파일 파싱
    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error& e) {
        // JSON 문법 깨짐/파싱 실패
        return false;
    }

    if (j.is_null()) return false;

	mapData.name = j["name"];
	mapData.mapID = j["mapId"];
	
    // Json 파일에서 몬스터 데이터 읽어오기
    LoadMonster(j, mapData.MonstersData);

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] Map ID [%d]", __FUNCTION__, __LINE__, mapId);
    return true;
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


 void MapManager::RemoveMap()
 {
    while(!m_destroyQueue.empty())
    {
        int mapId = m_destroyQueue.front();
        m_destroyQueue.pop();

        auto it = m_maps.find(mapId);
        if(it != m_maps.end())
        {
            K_slog_trace(K_SLOG_TRACE, "[%s][%d] Map Delete [%d]", __FUNCTION__, __LINE__, mapId);
            delete it->second;
            it->second = nullptr;
            m_maps.erase(it);
        }
    }
 }