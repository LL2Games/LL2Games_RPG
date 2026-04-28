#pragma once
#include "Player.h"
#include "MySqlConnectionPool.h"

class TradeService
{
public:
    TradeService();
    ~TradeService();

    int Request(Player* requester, Player* target_player, std::string &errMsg);
    int Execute(Player* requester, Player* accepter, std::string &errMsg);

private:
    MySqlConnectionPool* m_mySql;

};