#pragma once

#include "common.h"
#include "Player.h"
#include "Monster.h"
#include "DropInfos.h"

class ItemPacketSender
{
public:
    static void SendSpawnItem(std::vector<DropSpawnInfo> spawnedInfos, std::unordered_map<int, Player*>& playerList);
    static void SendAddItem(Player* player, std::vector<AddItemResult>& addItemResult);
    static void SendRemoveDropItem(const std::vector<int>& removeItems, const std::unordered_map<int, Player*>& playerList);
private:
};