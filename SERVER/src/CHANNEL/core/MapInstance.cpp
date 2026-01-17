#include "common.h"
#include "MapInstance.h"
#include "Monster.h"
#include <nlohmann/json.hpp>
#include <fstream>





MapInstance::MapInstance()
{

}

MapInstance::~MapInstance()
{

}

int MapInstance::Init(const MapInitData& data)
{
    this->m_mapID = data.mapID;
    this->m_monsterSpawnList = data.MonstersData;

    return 1;
}

int MapInstance::Update()
{
   
    return 1;
}

int MapInstance::InitSpawnMonster()
{
    m_monsterList.reserve(m_monsterSpawnList.size());
    for(m_monsterSpawnListIter = m_monsterSpawnList.begin(); m_monsterSpawnListIter < m_monsterSpawnList.end(); ++m_monsterListIter)
    {
        /*후에 추가 */
        Monster monster;
        //monster.type = m_monsterSpawnListIter->type;
        //monster.Pos = m_monsterSpawnListIter->spawnPos;

        // JSON 파일을 통해서 몬스터의 정보를 가지고 온다.
        // monsterManager를 통해서 불러와야 하지만 일단은 하드코딩으로 설정

        //m_monsterSpawnListIter
        Vec2 monsterPos;
        monsterPos.xPos = 100;
        monsterPos.yPos = 150;
        monster.SetPos(monsterPos);
        m_monsterList.push_back(monster);
    }

    return 1;
}   

int MapInstance::LoadMonsters(int monster_id)
 {
    /*
    struct MonsterSpawnData {
    MonsterType type;
    bool isAlive;
    float timer;
    float respawnDelay;
};
    */
    // MapManager에서 Map 생성 시 읽어온 몬스터 ID를 바탕으로 몬스터 정보가 적힌 Json 파일을 읽어온다.
    std::string path= "./data/Monsters/" + std::to_string(monster_id) +".json";
    std::ifstream file(path);

    if(!file.is_open()) {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return -1;
    }

    // JSON 파일 파싱
    nlohmann::json j;
    file >> j;

    



/*    
     auto MonsterCount = j["Monster"].size();

    // Json 파일에서 Monster 배열의 정보들을 반복문을 통해 설정
    for(const auto& Monster : j["monsters"]){
        MonsterSpawnData data;

        data.type = Monster["monsterId"];
        data.spawnPos.xPos = Monster["x"];
        data.spawnPos.yPos = Monster["y"];
        data.ItemId = Monster["group"];
        
    }
*/
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] MonsterLoad Success", __FUNCTION__, __LINE__);
 }

int MapInstance::SpawnMonster()
{
    return 1;
}

int MapInstance::RemoveMonster()
{
    return 1;
}