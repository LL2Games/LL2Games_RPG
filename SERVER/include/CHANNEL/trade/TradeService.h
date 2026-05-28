#pragma once
#include "Player.h"
#include "MySqlConnectionPool.h"
//#include "RedisClient.h"
#include <vector>
#include <unordered_map>
struct TradeItem
{
    std::string id;
    std::string type;
    size_t amount;
    size_t slot_index;
};
struct TradeSession
{
    Player* a_player;
    Player* b_player;

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
    Player* GetTargetPlayer(Player *);

private:
    int Execute(TradeSession *);
    int DecreaseItem(MYSQL *conn, const std::string &char_id, const TradeItem &item);
    int IncreaseItem(MYSQL *conn, const std::string &char_id, const TradeItem &item);

public:
    TradeService();
    ~TradeService();

    int Request(Player* requester, Player* target_player, std::string &errMsg);
    int Start(Player* requester, Player* accepter, std::string &errMsg);
    int UploadItem(Player*, const TradeItem&, std::string &errMsg);
    int Ready(Player*, const std::vector<TradeItem>&, std::string &errMsg);
    int Cancel(Player* requester, std::string &errMsg);

private:
    MySqlConnectionPool* m_mySql;
    //RedisClient* m_redis;
};