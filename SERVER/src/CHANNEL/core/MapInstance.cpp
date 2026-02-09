#include "common.h"
#include "MapInstance.h"
#include "Monster.h"
#include <nlohmann/json.hpp>
#include <fstream>

#define MAPDELETELIMIT 5

MapInstance::MapInstance() : m_limit(std::chrono::minutes{MAPDELETELIMIT})
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

	if(!m_has_player)
	{
		RemoveMap();
	}
	
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
			
        m_monsterList.push_back(monster);
    }

    return 1;
}   

int MapInstance::SpawnMonster()
{
	auto now = std::chrono::steady_clock::now();
	
	K_slog_trace(K_SLOG_ERROR, "[%s][%d] 몬스터 리스폰 시작", __FUNCTION__, __LINE__);
	
	
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

void MapInstance::OnEnter(int PlayerID, Player* player)
{
	auto it = m_playerList.find(PlayerID);

	if(it != m_playerList.end()) return;

	m_playerList[PlayerID] = player;

	if (m_playerCount == 0) {
        m_has_player = true;
        m_destroyRequested = false; // 혹시 남아있던 요청 초기화
        m_emptyTime = {};
    }
	m_playerCount++;
}

void MapInstance::OnLeave(int PlayerID)
{
	auto it = m_playerList.find(PlayerID);

	if(it == m_playerList.end()) return;

	m_playerList.erase(it);

	m_playerCount--;
	if(m_playerCount == 0)
	{
		m_emptyTime = std::chrono::steady_clock::now();
		m_has_player = false;
		m_destroyRequested = false;
	}
}

void MapInstance::HandleMonsterDead(Monster& monster)
{
	int killer = monster.GetLastAttackerID();
	
	GiveExp(killer, monster.GetExp());
	GiveItem(monster.GetItemGroup());
}

void MapInstance::GiveExp(int playerID, float exp)
{
	(void)playerID;
	(void)exp;
}


void MapInstance::GiveItem(int ItemGroup)
{
	(void)ItemGroup;
}



void MapInstance::HandleMove(Player* sender, Vec2 pos, float speed)
{
	if(!sender) return;

	K_slog_trace(K_SLOG_TRACE, "[%s][%d] 플레이어 ID [%d]", __FUNCTION__, __LINE__, sender->GetId());
	
	auto it = m_playerList.find(sender->GetId());

	if(it == m_playerList.end())
	{
		K_slog_trace(K_SLOG_ERROR, "[%s][%d] [%d]해당 맵에 존재하지 않은 플레이어 입니다.", __FUNCTION__, __LINE__, m_mapID);
		return;
	}

	sender->SetPosition(pos);

	BroadcastMoveExcept(sender, pos, speed);
}

// 각 플레이어한테 이동 정보 전달
void MapInstance::BroadcastMoveExcept(Player* sender, Vec2 pos, float speed)
{
	std::vector<std::string> payload;
	payload.reserve(4);
	payload.push_back(std::to_string(sender->GetId()));
	payload.push_back(std::to_string(pos.xPos));
	payload.push_back(std::to_string(pos.yPos));
	payload.push_back(std::to_string(speed));

	for(auto it = m_playerList.begin(); it != m_playerList.end(); ++it)
	{
		if(it->second == sender) continue;
		
		auto session = it->second->GetSession();
		
		session->Send(PKT_PLAYER_MOVE, payload);
	}
}

// 맵이 사라지는 경우 호출
void MapInstance::RemoveMap()
{
	auto now = std::chrono::steady_clock::now();

	if(!m_destroyRequested && (now - m_emptyTime) >= m_limit)
	{
		m_destroyRequested = true;
		K_slog_trace(K_SLOG_TRACE, "[%s][%d] [%d]맵 삭제", __FUNCTION__, __LINE__, m_mapID);
		if(m_onDestroyReq) m_onDestroyReq(m_mapID);
		
	}
}

