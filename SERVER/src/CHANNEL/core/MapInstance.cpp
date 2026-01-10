#include "CHANNEL/core/common.h"
#include "CHANNEL/core/MapInstance.h"



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
}

int MapInstance::Update(float deltaTime)
{
    
}

int MapInstance::InitSpawnMonster()
{
    m_monsterList.reserve(m_monsterSpawnList.size());
    for(m_monsterSpawnListIter = m_monsterSpawnList.begin(); m_monsterSpawnListIter < m_monsterSpawnList.end(); ++m_monsterListIter)
    {
        Monster monster;
        monster.type = m_monsterSpawnListIter->type;
        monster.Pos = m_monsterSpawnListIter->spawnPos;

        // JSON 파일을 통해서 몬스터의 정보를 가지고 온다.

        m_monsterList.push_back(monster);
    }

}   

int MapInstance::SpawnMonster()
{

}

int MapInstance::RemoveMonster()
{
    
}