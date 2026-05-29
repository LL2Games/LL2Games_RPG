#include "common.h"
#include "MapInstance.h"
#include "Monster.h"
#include "CommonEnum.h"
#include "IProjectile.h"
#include "timeUtility.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "MonsterPacketSender.h"
#include "PlayerPacketSender.h"
#include "ItemPacketSender.h"


#define MAPDELETELIMIT 5

MapInstance::MapInstance() : m_playerCount(0), m_limit(std::chrono::minutes{MAPDELETELIMIT}), m_combatService(nullptr)
{
	m_monsterManager = MonsterManager::GetInstance();
	m_dropManager = DropManager::GetInstance();
	m_contactCheckRadiusSq = 1000.0f; //임시 몸박 최소 데미지 거리
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
	//K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] MapInstance Pointer[%p]", __FILE__, __FUNCTION__, __LINE__, this);
	if(!m_has_player)
	{
		RemoveMap();
	}
	else
	{
		SpawnMonster();
		UpdateMonster();
		SendMapInfo();
		//m_projectileManager.Update(); //투사체 업데이트
		//ProcessRangedDamage(NowMs()); //원거리 공격 판정 및 데미지 처리
		ProcessContactDamage(NowMs()); //플레이어-몬스터 접촉 판정 및 데미지 처리
	}

    return 1;
}

//맵내에 있는 모든사용자에게 Update시 보내주는 정보
void MapInstance::SendMapInfo()
{
	for(auto it = m_playerList.begin(); it != m_playerList.end(); ++it)
	{
		SendMonsterMove(it->second);
	}
}

int MapInstance::InitSpawnMonster()
{
	int instanceId = 1;
    m_monsterList.reserve(m_monsterSpawnList.size());
	
    for(m_monsterSpawnListIter = m_monsterSpawnList.begin(); m_monsterSpawnListIter < m_monsterSpawnList.end(); ++m_monsterSpawnListIter)
    {
        Monster monster;
       
		
		if(!m_monsterManager->EnsureLoaded(m_monsterSpawnListIter->monsterId))
		{
			K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, m_monsterSpawnListIter->monsterId);
			return -1;
		}
			
		
		auto monsterTemplate = m_monsterManager->GetMonsterData(m_monsterSpawnListIter->monsterId);
		
		// 맵에 스폰된 몬스터들 끼리 구별하기 위한 값
		m_monsterSpawnListIter->instanceId = instanceId;
		instanceId++;

		
		if(monsterTemplate.has_value())
		{
			(*monsterTemplate).mapId = m_mapID;
			(*monsterTemplate).mapInstance = this;
			monster.Init(*monsterTemplate, *m_monsterSpawnListIter);
		}else {
			K_slog_trace(K_SLOG_ERROR, "[%s][%d] MonsterTemplate Get Failed Monster_Id[%d] FILE", __FUNCTION__, __LINE__, m_monsterSpawnListIter->monsterId);
			return -1;
		}

        m_monsterList.push_back(monster);
		
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
	{
		std::lock_guard<std::mutex> lock(m_playerMutex);
		auto it = m_playerList.find(PlayerID);

		if(it != m_playerList.end()) return;
	}

	m_playerList[PlayerID] = player;

	if (m_playerCount == 0) {
        m_has_player = true;
        m_destroyRequested = false; // 혹시 남아있던 요청 초기화
        m_emptyTime = {};
    }
	m_playerCount++;
	K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] PlayerID(%d)", __FILE__, __FUNCTION__, __LINE__, PlayerID);
	K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] m_playerCount(%d)", __FILE__, __FUNCTION__, __LINE__, m_playerCount);
	// 들어온 플레이어 한테 몬스터 정보 전달
	SendMonsterSnapshot(player);
}

void MapInstance::OnLeave(int PlayerID)
{
	std::lock_guard<std::mutex> lock(m_playerMutex);
		
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

	//K_slog_trace(K_SLOG_TRACE, "[%s][%d] 플레이어 ID [%d]", __FUNCTION__, __LINE__, sender->GetId());
	
	{
		std::lock_guard<std::mutex> lock(m_playerMutex);
		auto it = m_playerList.find(sender->GetId());

		if(it == m_playerList.end())
		{
			K_slog_trace(K_SLOG_ERROR, "[%s][%d] [%d]해당 맵에 존재하지 않은 플레이어 입니다.", __FUNCTION__, __LINE__, m_mapID);
			return;
		}
	}

	sender->SetPos(pos);

	PlayerPacketSender::SendPlayersMove(sender, pos, speed, m_playerList);
}



void MapInstance::SendMonsterSnapshot(Player* player)
{
     if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] player is nullptr", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    std::vector<Monster*> aliveMonsters;
    aliveMonsters.reserve(m_monsterList.size());

    for (auto& monster : m_monsterList)
    {
        if (!monster.IsAlive())
            continue;

        aliveMonsters.push_back(&monster);
    }

    MonsterPacketSender::SendMonsterSnapShot(player, aliveMonsters);
}

 void MapInstance::SendMonsterMove(Player* player)
 {
	  if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] player is nullptr", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    std::vector<Monster*> aliveMonsters;
    aliveMonsters.reserve(m_monsterList.size());

    for (auto& monster : m_monsterList)
    {
        if (!monster.IsAlive())
            continue;
		monster.SetState(MonsterState::E_Move);
        aliveMonsters.push_back(&monster);
    }
	MonsterPacketSender::SendMonsterMove(player, aliveMonsters);
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

// 나중에 PacketSender로 옮겨야함
void MapInstance::ResolveSkillHit(Player* Attacker, SkillDef& skillDef, std::vector<std::pair<Monster*, int>> hits)
{
	//skillDef는 상태이상 적용 시 필요한 정보 : 현재는 필요 없지만 후에 필요할지 몰라서 일단 매개변수로 추가해놓음

	K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] 공격 성공 ! 데이터 전송 준비 중.\n", __FILE__, __FUNCTION__, __LINE__);
	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] MapInstance Pointer[%p]", __FILE__, __FUNCTION__, __LINE__, this);

	std::vector<MonsterHitResult> results;
    results.reserve(hits.size());

	 K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] ResolveSkillHit hits size = %zu\n", __FILE__, __FUNCTION__, __LINE__, hits.size());

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
    K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] SendMonsterOnDamaged.\n", __FILE__, __FUNCTION__, __LINE__);    
	MonsterPacketSender::SendMonsterOnDamaged(Attacker, skillDef.skill_id, results, m_playerList);
}


void MapInstance::ProcessContactDamage(int64_t nowMs)
{
	//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]START", __FILE__, __FUNCTION__, __LINE__);
	{
		std::lock_guard<std::mutex> lock(m_playerMutex);
	   for(auto p : m_playerList)
	   {
			Player* player = p.second;

			// 플레이어가 죽었다면 스킵
			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]player->IsAlive()", __FILE__, __FUNCTION__, __LINE__);
			if(!player || !player->IsAlive()) continue;

			// 플레이어가 이미 피격 당했고 무적 상태라면 스킵
			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]CanTakeAnyContactDamage", __FILE__, __FUNCTION__, __LINE__);
			if(!player->CanTakeAnyContactDamage(nowMs)) continue;

			const Vec2 player_pos = player->GetPos();

			for(Monster& monster : m_monsterList)
			{
				// 몬스터가 죽은 상태라면 스킵
				//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]monster.IsAlive", __FILE__, __FUNCTION__, __LINE__);
				if(!monster.IsAlive()) continue;

			const Vec2 monster_pos = monster.GetPos();
			// 플레이어와 몬스터의 거리가 일정 이상이라면 스킵
			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]m_contactCheckRadiusSq [%f]", __FILE__, __FUNCTION__, __LINE__, m_contactCheckRadiusSq);
			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]Distancesquare(player_pos, monster_pos) [%f]", __FILE__, __FUNCTION__, __LINE__, Distancesquare(player_pos, monster_pos));
			if(Distancesquare(player_pos, monster_pos) > m_contactCheckRadiusSq) continue;
			
			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]Intersects", __FILE__, __FUNCTION__, __LINE__);
			 // 정밀 충돌(AABB/원형)
            if (!Collision::Intersects(player_pos, player->GetCollider(), monster_pos, monster.GetCollider())) continue;

			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]ApplyContactDamage", __FILE__, __FUNCTION__, __LINE__);
			int dmg = m_combatService->ApplyContactDamage(player, monster);
			
			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]OnDamaged", __FILE__, __FUNCTION__, __LINE__);
			player->OnDamaged(dmg,nowMs);

			PlayerHitResult result;

			result.damage = dmg;
			SetPlayerHitResult(player, monster.GetInstanceId(), result);

			//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]SendPlayerOnDamaged", __FILE__, __FUNCTION__, __LINE__);
			PlayerPacketSender::SendPlayerOnDamaged(player, result, m_playerList);
			}
		}
	}
	//K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d]END\n\n", __FILE__, __FUNCTION__, __LINE__);
}


/*gunoo22 260223 원거리 공격 처리*/
void MapInstance::ProcessRangedDamage(int64_t nowMs)
{
	{
		std::lock_guard<std::mutex> lock(m_playerMutex);
   		for(auto p : m_playerList)
   		{
				Player* player = p.second;
		
				// 플레이어가 죽었다면 스킵
				if(!player || !player->IsAlive()) continue;
		
				// 플레이어가 이미 피격 당했고 무적 상태라면 스킵
				if(!player->CanTakeAnyContactDamage(nowMs)) continue;
		
				const Vec2 player_pos = player->GetPos();
		
				K_slog_trace(K_SLOG_TRACE, "\n\n[%s:%s][%d] PLAYER POS [%f, %f]", __FILE__, __FUNCTION__, __LINE__, player_pos.xPos, player_pos.yPos);
				for (const auto& p : m_projectileManager.GetProjectiles())
				{
					const Vec2& projectile_pos = p->GetPos();
				
					K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] PROJECTILE POS [%f, %f]", __FILE__, __FUNCTION__, __LINE__, projectile_pos.xPos, projectile_pos.yPos);
				
					//플레이어와 투사체 거리가 일정거리 이상이라면 스킵
					if (Distancesquare(player_pos, projectile_pos) > m_contactCheckRadiusSq) continue;
				
					 // 정밀 충돌(AABB/원형)
					if (!Collision::Intersects(player_pos, player->GetCollider(), projectile_pos, p->GetCollider())) continue;
				
					//플레이어 온데미지
					player->OnDamaged(p->GetDamage(), nowMs);
				}
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

	ItemPacketSender::SendSpawnItem(m_spawnInfos, m_playerList);

	return true;
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
	
	ItemPacketSender::SendRemoveItem(removeItems, m_playerList);
}


