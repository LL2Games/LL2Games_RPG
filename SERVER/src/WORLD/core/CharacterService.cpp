#include "CharacterService.h"
#include "K_slog.h"
#include "RedisClient.h"
#include "MySqlConnectionPool.h"

CharacterService::CharacterService()
{
    m_db = MySqlConnectionPool::GetInstance();
}

CharacterService::~CharacterService()
{

}

std::vector<std::string> CharacterService::GetCharacterList(const std::string& account_id)
{
    std::vector<std::string> char_list;
    std::string query;

    //test
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] id[%s]", __FUNCTION__, __LINE__, account_id.c_str());

    //1.Redis charlist 조회
    //2.Redis miss-> MySQL 조회
    MYSQL* conn = m_db->GetConnection();
    if (!conn)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] MYSQL GetConnection failed", __FUNCTION__, __LINE__);
        goto err; 
    }

    query = std::string("SELECT char_id, name, level, job FROM `character` WHERE account_id ='" + account_id + "'");
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] query[%s]", __FUNCTION__, __LINE__, query.c_str());

    if (mysql_query(conn, query.c_str()) == 0)
    {
        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row;

        while ((row = mysql_fetch_row(res)))
        {
            std::string char_id = row[0];
            std::string summary = std::string(row[1]) + "$" + row[2] + "$" + row[3];
            char_list.push_back(summary);

            //Redis캐시 적재
        }
        mysql_free_result(res);
    }

    m_db->ReleaseConnection(conn);

    //test
    for (auto list: char_list)
    {
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] list[%s]", __FUNCTION__, __LINE__, list.c_str());
    }

    //3.응답값 리턴
err:
    return char_list;
}