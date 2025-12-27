#include "CHANNEL/db/PlayerService.h"
#include "K_slog.h"
#include <cstdio>
#include <sstream>

PlayerService::PlayerService()
{
    m_pool = MySqlConnectionPool::GetInstance();
}

PlayerService::~PlayerService()
{
}

std::unique_ptr<Player> PlayerService::LoadPlayer(int characterId)
{
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL* conn = m_pool->GetConnection();
    
    if(!conn) return nullptr;

    std::string query = "SELECT * FROM character WHERE char_id = " + std::to_string(characterId);

    int result = mysql_query(conn, query.c_str());
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_pool->ReleaseConnection(conn);
        return nullptr;
    }

    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer ERROR [%s]", mysql_error(conn));
        m_pool->ReleaseConnection(conn);
        return nullptr;
    }

    std::unique_ptr<Player> player = nullptr;

    if((row = mysql_fetch_row(res)) != nullptr)
    {
        player = std::make_unique<Player>();
        player->SetId(std::stoi(row[0]));
        player->SetAccountId(row[1]);
        player->SetName(row[2]);
        player->SetLevel(std::atoi(row[3]));
        player->SetJob(std::atoi(row[4]));
        
        K_slog_trace(K_SLOG_TRACE, "LoadPlayer SUCCESS [%d]", player->GetId());    
    }

    mysql_free_result(res);
    m_pool->ReleaseConnection(conn);
    return player;
}