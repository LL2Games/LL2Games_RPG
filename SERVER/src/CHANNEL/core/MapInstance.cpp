#include "common.h"
#include "MapInstance.h"
#include "Monster.h"
#include <nlohmann/json.hpp>
#include <fstream>



MapInstance::MapInstance()
{
	m_monsterManager = MonsterManager::GetInstance();
}

MapInstance::~MapInstance()
{

}

int MapInstance::Init(const MapInitData& data)
{
    this->m_mapID = data.mapID;
	// 여기서 Map Json 파일에서 읽어온 몬스터 정보 저장
    this->m_monsterSpawnList = data.MonstersData;

    return 1;
}

int MapInstance::Update()
{
	
	SpawnMonster();
	
	
	
    return 1;
}

int MapInstance::InitSpawnMonster()
{

    m_monsterList.reserve(m_monsterSpawnList.size());
	
    for(m_monsterSpawnListIter = m_monsterSpawnList.begin(); m_monsterSpawnListIter < m_monsterSpawnList.end(); ++m_monsterListIter)
    {
        Monster monster;
       
		if(!m_monsterManager->EnsureLoaded(m_monsterSpawnListIter->monsterId))
		{
			K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, m_monsterSpawnListIter->monsterId);
			return -1;
		}
			
		
		auto monsterTemplate = m_monsterManager->GetMonsterData(m_monsterSpawnListIter->monsterId);
		
		if(monsterTemplate)
		{
			monster.Init(*monsterTemplate, *m_monsterSpawnListIter);
		}else {
			K_slog_trace(K_SLOG_ERROR, "[%s][%d] MonsterTemplate Get Failed Monster_Id[%d] FILE", __FUNCTION__, __LINE__, m_monsterSpawnListIter->monsterId);
			return -1;
		}
		

		monster.SetPos(m_monsterSpawnListIter->spawnPos);
		
        m_monsterList.push_back(monster);
    }

    return 1;
}   

int MapInstance::SpawnMonster()
{
	auto now = std::chrono::steady_clock::now();
	
	K_slog_trace(K_SLOG_ERROR, "[%s][%d] 몬스터 리스폰 시작 FILE", __FUNCTION__, __LINE__);
	
	
	for(auto& Monsters : m_monsterList) 
	{
		if(Monsters.IsAlive()) continue;
		
		if(Monsters.CheckRespawnTime(now))
		{
			Monsters.Reset();
		}
	}
	
    return 1;
}

// 맵이 사라지는 경우 호출
int MapInstance::RemoveMonster()
{
    return 1;
}