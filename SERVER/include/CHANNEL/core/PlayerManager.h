#pragma once
#include "CHANNEL/core/common.h"
#include "CHANNEL/core/Player.h"
#include <unordered_map>
#include <memory>

class PlayerManager
{
    
private:
    std::unordered_map<int, std::unique_ptr<Player>> m_players;
public:
    PlayerManager();
    ~PlayerManager();

    bool Init();
    bool AddPlayer(std::unique_ptr<Player> player)
    {
        int playerId = player->GetId();

        if(m_players.find(playerId) != m_players.end())
        {
            return false; 
        }
     return true;
    }
    

};
