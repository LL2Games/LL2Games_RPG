#pragma once
#include "common.h"
#include "PlayerManager.h"
#include "CommonEnum.h"
#include "Monster.h"
#include "MonsterManager.h"
#include <nlohmann/json.hpp>



class MapInstance 
{
public:
    MapInstance();
    ~MapInstance();
    
    int Init(const MapInitData& data);
    int InitSpawnMonster();
    int SetMonster(MonsterType monsterType);
    //int LoadMonsters(int monster_id);
    int Update();
    int SpawnMonster();
    int RemoveMonster();
	
	int checkPlayer();

private:
   	// 플레이어 존재 여부
    bool  m_has_player;
	
	// 현재 맵에 플레이어 수
	uint16_t  m_playerCount;
    uint16_t m_mapID;
   
    std::vector<MonsterSpawnData> m_monsterSpawnList;
    std::vector<MonsterSpawnData>::iterator m_monsterSpawnListIter;

    std::vector<Monster> m_monsterList;
    std::vector<Monster>::iterator m_monsterListIter;
	
	MonsterManager* m_monsterManager;
};