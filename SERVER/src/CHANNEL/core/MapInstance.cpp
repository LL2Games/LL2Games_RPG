#include "CHANNEL/core/common.h"
#include "CHANNEL/core/MapInstance.h"
#include "CHANNEL/core/Monster.h"




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

int MapInstance::Update(float deltaTime)
{
    (void)deltaTime;
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
        Vec2 monsterPos;
        monsterPos.xPos = 100;
        monsterPos.yPos = 150;
        monster.SetPos(monsterPos);
        m_monsterList.push_back(monster);
    }

    return 1;
}   

int MapInstance::SpawnMonster()
{
    return 1;
}

int MapInstance::RemoveMonster()
{
    return 1;
}