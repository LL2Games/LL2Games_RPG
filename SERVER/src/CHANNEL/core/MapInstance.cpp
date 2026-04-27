#include "common.h"
#include "MapInstance.h"
#include "Monster.h"
#include "CommonEnum.h"
#include "timeUtility.h"
#include <nlohmann/json.hpp>
#include <fstream>


#define MAPDELETELIMIT 5

MapInstance::MapInstance() : m_playerCount(0), m_limit(std::chrono::minutes{MAPDELETELIMIT}), m_combatService(nullptr)
{
	m_monsterManager = MonsterManager::GetInstance();
	m_dropManager = DropManager::GetInstance();
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
		ProcessContactDamage(NowMs());
	}

    return 1;
}

//맵내에 있는 모든사용자에게 Update시 보내주는 정보
void MapInstance::BroadcastMapInfo()
{
	std::vector<std::string> payload;	

	//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]m_monsterList[size:%d]", __FILE__, __FUNCTION__, __LINE__, m_monsterList.size());
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
	
	
	//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d]m_playerList[size:%d]", __FILE__, __FUNCTION__, __LINE__, m_playerList.size());
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


void MapInstance::GiveExp(int playerID, float exp)
{
	(void)playerID;
	(void)exp;
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
		// 몬스터가 죽었을 때 아이템 드롭
		if(isDead)
		{
			// dropItem 세팅
			std::vector<DropResult> dropItems = m_dropManager->SetDropItem(m->GetCommonItemGroupID(), m->GetUniqueItemGroupID());
			// 아이템 스폰
			SpawnDropItem(m, dropItems);
		}	
		results.push_back({m->GetInstanceId(), dmg, m->GetCurrentHP(), m->GetMaxHP(), isDead});
	}
        
	BroadcastMonsterHit(Attacker, skillDef.skill_id, results);
}

void MapInstance::BroadcastMonsterHit(Player* Attacker, int SkillID, std::vector<MonsterHitResult> result)
{
	std::vector<std::string> payload;
	payload.reserve(result.size());

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

void MapInstance::BroadcastPlayerHit(Player* Defender, PlayerHitResult result)
{
	  for (auto& [id, p] : m_playerList)
    		{
        	if (!p) continue;

        	auto session = p->GetSession();
        	if (!session) continue;

        	std::vector<std::string> payload;
        	payload.reserve(6);

        	if (p == Defender)
        	{
        	    // 본인: 상세
        	    payload.push_back(std::to_string(result.player_id));
        	    payload.push_back(std::to_string(result.attacker_instance_id));
        	    payload.push_back(std::to_string(result.damage));
        	    payload.push_back(std::to_string(result.cur_hp));
        	    payload.push_back(std::to_string(result.max_hp));
        	    payload.push_back(std::to_string(static_cast<int>(result.state)));
        	}
        	else
        	{
        	    // 타인: 최소(HP 미공개)
        	    payload.push_back(std::to_string(result.player_id));
        	    payload.push_back(std::to_string(result.attacker_instance_id));
        	    payload.push_back(std::to_string(result.damage));
        	    payload.push_back(std::to_string(static_cast<int>(result.state)));
        	}

        	session->Send(PKT_PLAYER_ONDAMAGED, payload);
    }	
}

void MapInstance::ProcessContactDamage(int64_t nowMs)
{

   for(auto p : m_playerList)
   {
		Player* player = p.second;

		// 플레이어가 죽었다면 스킵
		if(!player || !player->IsAlive()) continue;

		// 플레이어가 이미 피격 당했고 무적 상태라면 스킵
		if(!player->CanTakeAnyContactDamage(nowMs)) continue;

		const Vec2 player_pos = player->GetPos();

		for(Monster& monster : m_monsterList)
		{
			// 몬스터가 죽은 상태라면 스킵
			if(!monster.IsAlive()) continue;

			const Vec2 monster_pos = monster.GetPos();
			// 플레이어와 몬스터의 거리가 일정 이상이라면 스킵
			if(Distancesquare(player_pos, monster_pos) > m_contactCheckRadiusSq) continue;
			
			 // 정밀 충돌(AABB/원형)
            if (!Collision::Intersects(player_pos, player->GetCollider(), monster_pos, monster.GetCollider())) continue;

			int dmg = m_combatService->ApplyContactDamage(player, monster);
			
			player->OnDamaged(dmg,nowMs);

			PlayerHitResult result;

			result.damage = dmg;
			SetPlayerHitResult(player, monster.GetInstanceId(), result);

			BroadcastPlayerHit(player, result);
		}
   }

}

void MapInstance::SetPlayerHitResult(Player* player, int monster_instanceId, PlayerHitResult& result)
{
	result.attacker_instance_id = monster_instanceId;
	result.player_id = player->GetId();
	result.cur_hp = player->GetCurHP();
	result.max_hp = player->GetMaxHP();
	result.state = player->GetState();
}


bool MapInstance::SpawnDropItem(Monster* monster, std::vector<DropResult> dropItems)
{
	
	for(size_t i =0; i < dropItems.size(); i++)
	{
		DropItems Item;
		Item.count = dropItems[i].count;
		Item.itemId = dropItems[i].itemId;
		Item.type = dropItems[i].type;
		Item.dropId = m_dropItems.size() + 1;
		Item.pos = monster->GetPos();
		Item.owner = monster->GetLastAttacker();
		Item.ownerExpireTimeMs = NowMs() + 60000; // 1분
		Item.expireTimeMs = NowMs() + 120000; // 2분

		m_dropItems[Item.itemId] = Item;

		DropSpawnInfo info;
		info.dropId = Item.dropId;
		info.count = Item.count;
		info.itemId = Item.itemId;
		info.type = Item.type;
		info.xPos = Item.pos.xPos;
		info.yPos = Item.pos.yPos;

		m_spawnInfos.push_back(info);
	}

	BroadcastDropSpawn(m_spawnInfos);

	return true;
}

void MapInstance::BroadcastDropSpawn(std::vector<DropSpawnInfo> spawnedInfos)
{
	for (auto& [id, p] : m_playerList)
    {
        if (!p) continue;
        auto session = p->GetSession();
        if (!session) continue;
        std::vector<std::string> payload;
		
		for(size_t i =0; i < spawnedInfos.size(); i++)
		{
        	payload.push_back(std::to_string(spawnedInfos[i].dropId));
        	payload.push_back(std::to_string(spawnedInfos[i].itemId));
        	payload.push_back(std::to_string(static_cast<int>(spawnedInfos[i].type)));
        	payload.push_back(std::to_string(spawnedInfos[i].count));
        	payload.push_back(std::to_string(spawnedInfos[i].xPos));
        	payload.push_back(std::to_string(spawnedInfos[i].yPos));
        	session->Send(PKT_DROPITEMS, payload);
		}
    }	
}


void MapInstance::CheckDropItem()
{
	int64_t nowMs = NowMs();
	std::vector<int> removeItems;

	for (auto it = m_dropItems.begin(); it != m_dropItems.end(); )
	{
    	auto& item = it->second;

    	if (item.owner != nullptr && item.ownerExpireTimeMs <= nowMs)
    	{
    	    item.owner = nullptr; // 누구나 먹을 수 있게
    	}

    	if (item.expireTimeMs <= nowMs)
    	{
			//삭제 아이템에 대한 정보 전달
			removeItems.push_back(it->second.dropId);
    	    it = m_dropItems.erase(it);
    	}
    	else
    	{
    	    ++it;
    	}
	}
	BroadcastRemoveItem(removeItems);
}


void MapInstance::BroadcastRemoveItem(std::vector<int> removeItems)
{
	for (auto& [id, p] : m_playerList)
    {
        if (!p) continue;
        auto session = p->GetSession();
        if (!session) continue;
        std::vector<std::string> payload;
		
		for(size_t i =0; i < removeItems.size(); i++)
		{
        	payload.push_back(std::to_string(removeItems[i]));

        	session->Send(PKT_REMOVEITEMS, payload);
		}
    }	
}