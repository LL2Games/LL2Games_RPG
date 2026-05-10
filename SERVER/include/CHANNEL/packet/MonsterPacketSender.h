#pragma once

#include "common.h"
#include "Player.h"
#include "Monster.h"

class MonsterPacketSender
{
public:
    static void SendMonsterSnapShot(Player* player, const std::vector<Monster*>& monsters);
    static void SendMonsterMove(Player* player, const std::vector<Monster*>& monsters);
    static void SendMonsterOnDamaged(Player* Attacker, int SkillID, std::vector<MonsterHitResult>& result, std::unordered_map<int, Player*>& playerList);
private:
};