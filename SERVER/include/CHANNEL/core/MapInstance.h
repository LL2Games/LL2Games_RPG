#pragma once
#include "CHANNEL/core/common.h"
#include "CHANNEL/core/PlayerManager.h"
#include "COMMON/CommonEnum.h"
#include "CHANNEL/core/Monster.h"
#include <nlohmann/json.hpp>


class MapInstance 
{
public:
    MapInstance();
    ~MapInstance();
    
    int Init(const MapInitData& data);
    int InitSpawnMonster();
    int SetMonster(MonsterType monsterType);
    int LoadMonsters(int monster_id);
    int Update();
    int SpawnMonster();
    int RemoveMonster();

private:
    //uint16_t  m_map_id;
    bool  m_has_player;
    MapRegion m_mapID;
   
    std::vector<MonsterSpawnData> m_monsterSpawnList;
    std::vector<MonsterSpawnData>::iterator m_monsterSpawnListIter;

    std::vector<Monster> m_monsterList;
    std::vector<Monster>::iterator m_monsterListIter;
};