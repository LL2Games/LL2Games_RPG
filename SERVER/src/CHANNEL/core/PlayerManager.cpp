#include "PlayerManager.h"

PlayerManager::PlayerManager()
{
    m_mySql = MySqlConnectionPool::GetInstance();
}

PlayerManager::~PlayerManager()
{
}


bool PlayerManager::AddPlayer(std::unique_ptr<Player> player)
 {
    int playerId = player->GetId();

    if(m_players.find(playerId) != m_players.end())
    {
        return false; 
    }
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] Player character_id [%d]", __FUNCTION__ , __LINE__, player->GetId());
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] Player character_id [%s]", __FUNCTION__ , __LINE__, player->GetName().c_str());

    m_players[playerId] = move(player);
    return true;
}




Player* PlayerManager::GetPlayer(int playerID)
{
    auto it = m_players.find(playerID);

    if(it == m_players.end()) return nullptr;

    return it->second.get();
}

//추후에는 Redis먼저 조회 하고, Redis의 특정 주기에 따라서 DB조회를 해야함
//바로 DB조회 하는 방식은 리소스 손해를 많이본다.
Player* PlayerManager::GetPlayer(const std::string& playerName)
{
    MYSQL *conn = m_mySql->GetConnection();
    std::string query;
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    int playerId = 0;
    int result = 0;
    if (!conn)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] MYSQL GetConnection failed", __FILE__, __LINE__);
        return nullptr;
    }
    query = "SELECT char_id FROM `character` WHERE name = '" + playerName + "'";
    K_slog_trace(K_SLOG_DEBUG, "SQL [%s]", query.c_str());

    result = mysql_query(conn, query.c_str());
    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d]SELECT FAIL: %s", __FILE__, __LINE__, mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "GetPlayer ERROR [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    if((row = mysql_fetch_row(res)) != nullptr)
    {
        playerId = std::atoi(row[0]);
        K_slog_trace(K_SLOG_TRACE, "[%s][%d]GetPlayer SUCCESS: playerId=%d ", __FILE__, __LINE__, playerId);
    }
    else
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d]GetPlayer FAIL: playerName=%s ", __FILE__, __LINE__, playerName.c_str());
    }
    m_mySql->ReleaseConnection(conn);

    auto it = m_players.find(playerId);
    if(it == m_players.end()) return nullptr;
    return it->second.get();
}

bool PlayerManager::RemovePlayer(int playerId)
{
    auto it = m_players.find(playerId);

    if(it == m_players.end()) return false;

    m_players.erase(it);
    return true;
}