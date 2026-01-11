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
    
    std::unique_ptr<Player> LoadPlayer(int characterId);
    bool SavePlayer(std::unique_ptr<Player> player);
    bool UpdatePlayerPosition(int playerId, float x, float y);

private:
    MySqlConnectionPool* m_mySql;
    RedisClient* m_redis;

};