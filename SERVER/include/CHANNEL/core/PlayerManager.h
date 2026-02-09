#pragma once
#include "common.h"
#include "CommonEnum.h"
#include <unordered_map>
#include <memory>
#include "Player.h"

class PlayerManager
{   
public:
    PlayerManager();
    ~PlayerManager();

    bool Init();
    bool AddPlayer(std::unique_ptr<Player> player);

    bool MovePlayer(int playerID, Vec2 pos, float speed);
public:
    Player* GetPlayer(int playerID);


private:
    std::unordered_map<int, std::unique_ptr<Player>> m_players;
     
    

};
