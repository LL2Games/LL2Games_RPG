#pragma once
#include "Player.h"
#include "MySqlConnectionPool.h"
#include <vector>
#include <unordered_map>
struct TradeItem
{
    std::string id;
    size_t amount;
};
struct TradeSession
{
    int a_id;
    int b_id;

    bool a_ready;
    bool b_ready;

    std::vector<TradeItem> a_items;
    std::vector<TradeItem> b_items;
};
class TradeService
{
private:
    static std::unordered_map<int, TradeSession*> m_sessions; //<player_id, session*>
    void CreateTradeSession(Player *, Player *);
    void DeleteTradeSession(TradeSession*);
public:
    TradeSession* GetTradeSession(Player *);

private:
    int Execute(TradeSession *);
public:
    TradeService();
    ~TradeService();

    int Request(Player* requester, Player* target_player, std::string &errMsg);
    int Start(Player* requester, Player* accepter, std::string &errMsg);
    int Ready(Player*, const std::vector<TradeItem>&, std::string &errMsg);

private:
    MySqlConnectionPool* m_mySql;

};