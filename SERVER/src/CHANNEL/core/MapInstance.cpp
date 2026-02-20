#include "common.h"
#include "MapInstance.h"
#include "Monster.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "CommonEnum.h"

#define MAPDELETELIMIT 5

MapInstance::MapInstance() : m_playerCount(0), m_limit(std::chrono::minutes{MAPDELETELIMIT})
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

#if 1 //guno22_TEST
	{
		//"100000000”
		std::vector<MonsterSpawnData>& list = this->m_monsterSpawnList;
		K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST size[%d]", __FUNCTION__, __LINE__, list.size());
		for (auto& m : list)
		{
			K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST m.monsterId[%d]", __FUNCTION__, __LINE__, m.monsterId);
		}
	}
#endif
	InitSpawnMonster();
    return 1;
}

int MapInstance::Update()
{
	if(!m_has_player)
	{
		RemoveMap();
	}
	else
	{
		SpawnMonster();
		UpdateMonster();
		BroadcastMapInfo();
	}

    return 1;
}

//맵내에 있는 모든사용자에게 Update시 보내주는 정보
void MapInstance::BroadcastMapInfo()
{
	std::vector<std::string> payload;	

	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]m_monsterList[size:%d]", __FILE__, __FUNCTION__, __LINE__, m_monsterList.size());
	for (auto& monster : m_monsterList)
	{
		if (monster.IsAlive())
		{
			payload.push_back(std::to_string(monster.GetInstanceId()));
			payload.push_back(std::to_string(monster.GetPos().xPos));
			payload.push_back(std::to_string(monster.GetPos().yPos));
			payload.push_back(std::to_string(monster.GetCurrentHP()));
			payload.push_back(std::to_string(monster.GetMaxHP()));
		}
	}
	
	
	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]m_playerList[size:%d]", __FILE__, __FUNCTION__, __LINE__, m_playerList.size());
	for(auto it = m_playerList.begin(); it != m_playerList.end(); ++it)
	{	
		auto session = it->second->GetSession();	
		session->Send(PKT_MONSTER_MOVE, payload);
	}
}

int MapInstance::InitSpawnMonster()
{
	int instanceId = 1;
    m_monsterList.reserve(m_monsterSpawnList.size());
	
	
	//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]gunoo22_TEST", __FILE__, __FUNCTION__, __LINE__);
    for(m_monsterSpawnListIter = m_monsterSpawnList.begin(); m_monsterSpawnListIter < m_monsterSpawnList.end(); ++m_monsterSpawnListIter)
    {
        Monster monster;
       
		//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]gunoo22_TEST", __FILE__, __FUNCTION__, __LINE__);
		if(!m_monsterManager->EnsureLoaded(m_monsterSpawnListIter->monsterId))
		{
			K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, m_monsterSpawnListIter->monsterId);
			return -1;
		}
		//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]gunoo22_TEST", __FILE__, __FUNCTION__, __LINE__);	
		
		auto monsterTemplate = m_monsterManager->GetMonsterData(m_monsterSpawnListIter->monsterId);
		
		// 맵에 스폰된 몬스터들 끼리 구별하기 위한 값
		m_monsterSpawnListIter->instanceId = instanceId;
		instanceId++;

		//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]gunoo22_TEST", __FILE__, __FUNCTION__, __LINE__);
		if(monsterTemplate.has_value())
		{
			(*monsterTemplate).mapId = m_mapID;
			monster.Init(*monsterTemplate, *m_monsterSpawnListIter);
		}else {
			K_slog_trace(K_SLOG_ERROR, "[%s][%d] MonsterTemplate Get Failed Monster_Id[%d] FILE", __FUNCTION__, __LINE__, m_monsterSpawnListIter->monsterId);
			return -1;
		}
		//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]gunoo22_TEST", __FILE__, __FUNCTION__, __LINE__);
			
        m_monsterList.push_back(monster);
		//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]gunoo22_TEST", __FILE__, __FUNCTION__, __LINE__);
    }

    return 1;
}   

int MapInstance::UpdateMonster()
{
	//K_slog_trace(K_SLOG_ERROR, "[%s][%d] 몬스터 업데이트 시작", __FUNCTION__, __LINE__);
	for(auto& monster : m_monsterList) 
	{
		if(monster.IsAlive())
		{
			monster.Update();
		}
	}

	return 1;
}

int MapInstance::SpawnMonster()
{
	auto now = std::chrono::steady_clock::now();
	
	//K_slog_trace(K_SLOG_TRACE, "[%s][%d] 몬스터 리스폰 시작", __FUNCTION__, __LINE__);
	for(auto& monster : m_monsterList) 
	{
		if(monster.IsAlive()) continue;
		
		if(monster.CheckRespawnTime(now))
		{
			monster.Reset();
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
	K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] PlayerID(%d)", __FILE__, __FUNCTION__, __LINE__, PlayerID);
	K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] m_playerCount(%d)", __FILE__, __FUNCTION__, __LINE__, m_playerCount);
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

	K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] PlayerID(%d)", __FILE__, __FUNCTION__, __LINE__, PlayerID);
	K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] m_playerCount(%d)", __FILE__, __FUNCTION__, __LINE__, m_playerCount);
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

	sender->SetPos(pos);

	BroadcastMoveExcept(sender, pos, speed);
}

// 각 플레이어한테 이동 정보 전달
void MapInstance::BroadcastMoveExcept(Player* sender, Vec2 pos, float speed)
{
	std::vector<std::string> payload;
	payload.reserve(4);
	K_slog_trace(K_SLOG_TRACE, "[%s][%d] playerID [%d]", __FUNCTION__, __LINE__, sender->GetId());
	K_slog_trace(K_SLOG_TRACE, "[%s][%d] xPos [%f]", __FUNCTION__, __LINE__, pos.xPos);
	K_slog_trace(K_SLOG_TRACE, "[%s][%d] yPos [%f]", __FUNCTION__, __LINE__, pos.yPos);
	K_slog_trace(K_SLOG_TRACE, "[%s][%d] speed [%f]", __FUNCTION__, __LINE__, speed);
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


void MapInstance::ResolveSkillHit(Player* Attacker, SkillDef& skillDef, std::vector<std::pair<Monster*, int>> hits)
{
	//skillDef는 상태이상 적용 시 필요한 정보 : 현재는 필요 없지만 후에 필요할지 몰라서 일단 매개변수로 추가해놓음
	(void)skillDef;

	std::vector<MonsterHitResult> results;
    results.reserve(hits.size());

	for (auto& [m, dmg] : hits)
	{
		bool isDead = m->OnDamaged(Attacker, dmg);	
		results.push_back({m->GetInstanceId(), dmg, m->GetCurrentHP(), m->GetMaxHP(), isDead});
	}
        
	BroadcastMonsterHit(Attacker, skillDef.skill_id, results);
}

void MapInstance::BroadcastMonsterHit(Player* Attacker, int SkillID, std::vector<MonsterHitResult> result)
{
	std::vector<std::string> payload;
	payload.reserve(4);

	payload.push_back(std::to_string(Attacker->GetId()));
	payload.push_back(std::to_string(SkillID));

	for (const auto& r : result)
	{
    	payload.push_back(std::to_string(r.monster_instance_id));
    	payload.push_back(std::to_string(r.damage));
    	payload.push_back(std::to_string(r.cur_hp));
    	payload.push_back(std::to_string(r.max_hp));
    	payload.push_back(r.dead ? "1" : "0");
	}
	for(auto it = m_playerList.begin(); it != m_playerList.end(); ++it)
	{
		//if(it->second == Attacker) continue;
		
		auto session = it->second->GetSession();
		
		session->Send(PKT_MONSTER_ONDAMAGED, payload);
	}

}


void MapInstance::ProcessContactDamage(int64_t nowMs)
{
   (void)nowMs;

   for(auto p : m_playerList)
   {
		Player* player = p.second;
		if(!player || !player->IsAlive()) continue;

		if(!player->CanTakeAnyContactDamage(nowMs)) continue;

		const Vec2 player_pos = player->GetPos();

	

		for(Monster& monster : m_monsterList)
		{
			if(!monster.IsAlive()) continue;

			const Vec2 monster_pos = monster.GetPos();
			if(Distancesquare(player_pos, monster_pos) > m_contactCheckRadiusSq) continue;
			



		}
   }

}