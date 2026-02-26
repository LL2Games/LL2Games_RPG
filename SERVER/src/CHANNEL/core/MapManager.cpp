#include "common.h"
#include "MapManager.h"
#include "PlayerManager.h"
#include "MapUpdateTask.h"
#include "ChannelServer.h"

#define MAP_PATH "../src/CHANNEL/data/Maps/"
#define UPDATE_INTERVAL 2000 //2초
namespace fs = std::filesystem;

MapManager::MapManager(ChannelServer *server) : m_server(server)
{
}

MapManager::~MapManager()
{
}

bool MapManager::Init()
{
    // 서버 구동 시 Map 데이터 전부 읽어서 미리 저장
    if(!PreLoadAll()) return false;
    return true;
}

void MapManager::Start()
{
    m_running = true;
    m_thread = std::thread(&MapManager::Update, this);
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] MapManager Update Start", __FILE__, __LINE__);
}

void MapManager::Stop()
{
    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void MapManager::Update()
{
    while (m_running)
    {
        for (auto iter = m_maps.begin(); iter != m_maps.end(); ++iter)
        {
            if (iter->second != nullptr)
            {
                MapInstance *map = iter->second;

                auto task = std::make_unique<MapUpdateTask>(map);
                m_server->GetThreadPool()->Submit(std::move(task));
            }
        }
        RemoveMap();
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL)); // 간격
    }
}

MapInstance *MapManager::GetOrCreate(int mapId)
{
    auto it = m_maps.find(mapId);

    if (it != m_maps.end())
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] alreay Map is Maked", __FUNCTION__, __LINE__);
        return it->second;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] MAPINIT START", __FUNCTION__, __LINE__);

    MapInitData mapData;

    auto itInit = m_maps_initData.find(mapId);
    if (itInit != m_maps_initData.end())
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);
        mapData = itInit->second;
    }
    else
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);
        if (!LoadJsonFile(mapId, mapData))
            return nullptr;
    }

    MapInstance *newMap = new MapInstance();

    newMap->SetCombatService(m_server->GetCombatService());

    if (newMap->Init(mapData) == 1)
    {
        m_maps[mapId] = newMap;
    }
    else
    {
        delete newMap;
        return nullptr;
    }

    // 맵 생성 후 삭제 예약을 걸어둔다. 맵에 플레이어가 없는 경우에 일정 시간이 지나면 맵을 삭제할 수 있도록 설정
    newMap->SetDestroyCallback([this](int id)
                               { m_destroyQueue.push(id); });

    return newMap;
}

bool MapManager::PreLoadAll()
{
    for (const auto &entry : fs::directory_iterator(MAP_PATH))
    {
        if (!entry.is_regular_file())
            continue;
        if (entry.path().extension() != ".json")
            continue;

        /*
            // entry.path() -> 파일 경로
            // entry.path().filename() -> 파일명
            // entry.path().stem() -> 확장자 제거한 파일명(예: "1001")

        */
        int map_id = std::stoi(entry.path().stem().string());

        auto it = m_maps_initData.find(map_id);
        if (it != m_maps_initData.end())
            continue;

        MapInitData mapData;

        bool is_Load = LoadJsonFile(map_id, mapData);
        if (!is_Load)
            return false;

        m_maps_initData.emplace(map_id, std::move(mapData));
    }

    return true;
}

bool MapManager::LoadJsonFile(int mapId, MapInitData &mapData)
{

    std::string path = MAP_PATH + std::to_string(mapId) + ".json";
    std::ifstream file(path);

    if (!file.is_open())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return false;
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] OPEN [%s] SUCCESS", __FUNCTION__, __LINE__, path.c_str());


    // JSON 파일 파싱
    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        // JSON 문법 깨짐/파싱 실패
        return false;
    }

    if (j.is_null())
        return false;

    mapData.name = j.at("name").get<std::string>();
    mapData.mapID = j.at("mapId").get<u_int32_t>();

    // Json 파일에서 몬스터 데이터 읽어오기
    LoadMonster(j, mapData.MonstersData);

    //gunoo22_TEST
    K_slog_trace(K_SLOG_TRACE, "[%s][%d]gunoo22_TEST mapData.nams[%s]", __FUNCTION__, __LINE__, mapData.name.c_str());
    K_slog_trace(K_SLOG_TRACE, "[%s][%d]gunoo22_TEST mapData.mapID[%d]", __FUNCTION__, __LINE__, mapData.mapID);


    K_slog_trace(K_SLOG_TRACE, "[%s][%d] Map ID [%d]", __FUNCTION__, __LINE__, mapId);
    return true;
}

void MapManager::LoadMonster(nlohmann::json &j, std::vector<MonsterSpawnData>& MonstersData)
{
    /*
    struct MonsterSpawnData {
    MonsterType type;
    bool isAlive;
    float timer;
    float respawnDelay;
};
    */

    const auto& arr = j.at("monsters");
    MonstersData.clear();
    MonstersData.reserve(arr.size());

    for (const auto& m : arr)
    {
        MonsterSpawnData data;
        data.monsterId = m.at("monsterId").get<int>();   // 키 맞춰라
        data.spawnPos.xPos = m.at("xPos").get<float>();     
        data.spawnPos.yPos = m.at("yPos").get<float>();
        data.ItemId = m.at("group").get<int>(); 

        MonstersData.push_back(std::move(data));
    }

    for (auto &md : MonstersData)
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d]gunoo22_TEST monsterid[%d]", __FUNCTION__, __LINE__, md.monsterId);
    }


    K_slog_trace(K_SLOG_TRACE, "[%s][%d] MonsterLoad Success", __FUNCTION__, __LINE__);
}

void MapManager::RemoveMap()
{
    while (!m_destroyQueue.empty())
    {
        int mapId = m_destroyQueue.front();
        m_destroyQueue.pop();

        auto it = m_maps.find(mapId);
        if (it != m_maps.end())
        {
            K_slog_trace(K_SLOG_TRACE, "[%s][%d] Map Delete [%d]", __FUNCTION__, __LINE__, mapId);
            delete it->second;
            it->second = nullptr;
            m_maps.erase(it);
        }
    }
}