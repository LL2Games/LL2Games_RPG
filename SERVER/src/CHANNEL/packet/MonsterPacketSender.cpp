#include "MonsterPacketSender.h"
#include "ChannelSession.h"


void MonsterPacketSender::SendMonsterSnapShot(Player* player, const std::vector<Monster*>& monsters)
{
    if (player == nullptr)
        return;

    auto session = player->GetSession();
    if (session == nullptr)
        return;

    std::vector<std::string> payload;
    payload.reserve(1 + monsters.size() * 8);

    payload.push_back(std::to_string(monsters.size()));

    for (auto* monster : monsters)
    {
        if (monster == nullptr)
            continue;

        payload.push_back(std::to_string(monster->GetId()));          // monsterTemplateId
        payload.push_back(std::to_string(monster->GetInstanceId()));  // monsterObjectId
        payload.push_back(std::to_string(monster->GetPos().xPos));
        payload.push_back(std::to_string(monster->GetPos().yPos));
        payload.push_back(std::to_string(monster->GetDir().xPos));
        payload.push_back(std::to_string(monster->GetDir().yPos));
        payload.push_back(std::to_string(monster->GetMoveSpeed()));
        payload.push_back(std::to_string(monster->GetCurrentHP()));
        payload.push_back(std::to_string(monster->GetMaxHP()));
        payload.push_back(std::to_string(static_cast<int>(monster->GetState())));
// 테스트용 로그
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] MonsterId [%d]", __FILE__, __FUNCTION__, __LINE__, monster->GetId());
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster InstanceId [%d]", __FILE__, __FUNCTION__, __LINE__, monster->GetInstanceId());
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster xPos [%f]", __FILE__, __FUNCTION__, __LINE__, monster->GetPos().xPos);
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster yPos [%f]", __FILE__, __FUNCTION__, __LINE__, monster->GetPos().yPos);
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster Dir [%d]", __FILE__, __FUNCTION__, __LINE__, monster->GetDir());
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster CurrentHp", __FILE__, __FUNCTION__, __LINE__, monster->GetCurrentHP());
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster State", __FILE__, __FUNCTION__, __LINE__, static_cast<int>(monster->GetState()));
    }

    K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster SnapShot Data Send", __FILE__, __FUNCTION__, __LINE__);
    session->Send(PKT_MONSTER_SNAPSHOT, payload);
}

void MonsterPacketSender::SendMonsterMove(Player* player, const std::vector<Monster*>& monsters)
{
     if (player == nullptr)
        return;

    auto session = player->GetSession();
    if (session == nullptr)
        return;

    std::vector<std::string> payload;
    payload.reserve(1 + monsters.size() * 8);

    payload.push_back(std::to_string(monsters.size()));

    for (auto* monster : monsters)
    {
        if (monster == nullptr)
            continue;

        payload.push_back(std::to_string(monster->GetInstanceId()));          // monster instanceid
        payload.push_back(std::to_string(static_cast<int>(monster->GetState())));
        payload.push_back(std::to_string(monster->GetDir().xPos));
        payload.push_back(std::to_string(monster->GetDir().yPos));
        payload.push_back(std::to_string(monster->GetPos().xPos));
        payload.push_back(std::to_string(monster->GetPos().yPos));
        payload.push_back(std::to_string(monster->GetCurrentHP()));
        payload.push_back(std::to_string(monster->GetMaxHP()));
// 테스트용 로그
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster InstanceId [%d]", __FILE__, __FUNCTION__, __LINE__, monster->GetInstanceId());
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster xPos [%f]", __FILE__, __FUNCTION__, __LINE__, monster->GetPos().xPos);
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster yPos [%f]", __FILE__, __FUNCTION__, __LINE__, monster->GetPos().yPos);
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster Dir [%d]", __FILE__, __FUNCTION__, __LINE__, monster->GetDir());
        //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster CurrentHp [%d]", __FILE__, __FUNCTION__, __LINE__, monster->GetCurrentHP());
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster State [%d]", __FILE__, __FUNCTION__, __LINE__, static_cast<int>(monster->GetState()));
    }

    //K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Monster Move Data Send", __FILE__, __FUNCTION__, __LINE__);
    session->Send(PKT_MONSTER_MOVE, payload);
}


void MonsterPacketSender::SendMonsterOnDamaged(Player* Attacker, int SkillID, std::vector<MonsterHitResult>& result, std::unordered_map<int, Player*>& playerList)
{
    std::vector<std::string> payload;
	payload.reserve(3 + result.size() * 5);

	payload.push_back(std::to_string(result.size()));
	payload.push_back(std::to_string(Attacker->GetId()));
	payload.push_back(std::to_string(SkillID));
	for (const auto& r : result)
	{
    	payload.push_back(std::to_string(r.monster_instance_id));
    	payload.push_back(std::to_string(r.damage));
    	payload.push_back(std::to_string(r.cur_hp));
    	payload.push_back(std::to_string(r.max_hp));
    	payload.push_back(r.dead ? "1" : "0");

            
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] damage [%d]", __FILE__, __FUNCTION__, __LINE__, r.damage);
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] cur_hp [%d]", __FILE__, __FUNCTION__, __LINE__, r.cur_hp);
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] max_hp [%d]", __FILE__, __FUNCTION__, __LINE__, r.max_hp);
	}

    
	for(auto it = playerList.begin(); it != playerList.end(); ++it)
	{
		//if(it->second == Attacker) continue;
		
		auto session = it->second->GetSession();
        if (session == nullptr)
            continue;
		
		session->Send(PKT_MONSTER_ONDAMAGED, payload);
	}
}


