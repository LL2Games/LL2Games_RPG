#pragma once

#include "common.h"
#include "Player.h"
#include "Monster.h"
#include "DropInfos.h"

class ItemPacketSender
{
public:
    static void SendSpawnItem(std::vector<DropSpawnInfo> spawnedInfos, std::unordered_map<int, Player*>& playerList);
    static void SendRemoveItem(std::vector<int> removeItems, std::unordered_map<int, Player*>& playerList);
private:
};