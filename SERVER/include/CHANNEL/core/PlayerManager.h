#pragma once
#include "CHANNEL/core/common.h"
#include "CHANNEL/core/Player.h"
#include <unordered_map>
#include <memory>

class PlayerManager
{   
public:
    PlayerManager();
    ~PlayerManager();

    bool Init();
    bool AddPlayer(std::unique_ptr<Player> player);

public:
    Player* GetPlayer(int playerID);


private:
    std::unordered_map<int, std::unique_ptr<Player>> m_players;
     
    

};
