#pragma once
#include "../core/common.h"
#include "../core/Player.h"
#include "MySqlConnectionPool.h"
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
    MySqlConnectionPool* m_pool;
};