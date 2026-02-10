#pragma once
#include "../core/common.h"
#include "../core/Player.h"
#include "../util/PlayerData.h"
#include "MySqlConnectionPool.h"
#include "RedisClient.h"
#include <memory>



class PlayerService
{
public:
    PlayerService();
    ~PlayerService();
    
    static std::unique_ptr<Player> LoadPlayer(int characterId);
    static bool SavePlayer(std::unique_ptr<Player> player);
    static bool UpdatePlayerPosition(int playerId, float x, float y);

private:
    static MySqlConnectionPool* m_mySql;
    static RedisClient* m_redis;
};