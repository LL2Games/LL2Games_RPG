#pragma once
#include "common.h"
#include "Player.h"
#include "Inventory.h"
#include "PlayerData.h"
#include "MySqlConnectionPool.h"
#include "RedisClient.h"
#include <memory>



class PlayerService
{
public:
    PlayerService();
    ~PlayerService();
    
    static std::unique_ptr<Player> LoadPlayer(int characterId);
    static bool LoadInventoryMeta(Player* player);
    static bool LoadInventory(Player* player);
    static bool LoadLearnedSkill(Player* player);
    static bool LoadSlotSetting(Player* player);
    static bool SavePlayer(std::unique_ptr<Player> player);
    static bool UpdatePlayerPosition(int playerId, float x, float y);

private:
    static MySqlConnectionPool* m_mySql;
    static RedisClient* m_redis;
    static std::unique_ptr<Player> m_player;
};