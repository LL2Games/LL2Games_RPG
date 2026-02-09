#include "PlayerStatRepository.h"
#include "MySqlConnectionPool.h"
#include "RedisClient.h"
#include "K_slog.h"

PlayerStatRepository::PlayerStatRepository()
{
    m_mySql = MySqlConnectionPool::GetInstance();
    m_redis = RedisClient::GetInstance();
}

PlayerStatRepository::~PlayerStatRepository()
{
}


//추후에는 Redis먼저 업데이트 하고, Redis의 특정 주기에 따라서 DB업데이트를 해야함
//바로 DB업데이트 하는 방식은 리소스 손해를 많이본다.
int PlayerStatRepository::Update(const std::string &charId, const std::string &statType, std::string &errMsg)
{
    MYSQL *conn = m_mySql->GetConnection();
    std::string query;
    my_ulonglong affected;
    int result = 0;
    if (!conn)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] MYSQL GetConnection failed", __FILE__, __LINE__);
        errMsg = "MYSQL GetConnection failed";
        return -1;
    }
    query = "UPDATE character_stat SET " + statType + " = " + statType + " + 1 WHERE char_id = " + charId;
    result = mysql_query(conn, query.c_str());
    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d]UPDATE FAIL: %s", __FILE__, __LINE__, mysql_error(conn));
        errMsg = std::string("UPDATE FAIL: ") + mysql_error(conn);
        return -1;
    }

    affected = mysql_affected_rows(conn);
    if (affected == (my_ulonglong)-1)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d]affected_rows ERROR: %s", __FILE__, __LINE__, mysql_error(conn));
        errMsg = std::string("affected_rows ERROR: ") + mysql_error(conn);
        return -1;
    }
    else if (affected == 0)
    {
         // char 없음 or 이미 같은 값
        K_slog_trace(K_SLOG_TRACE, "[%s][%d]No row updated (char_id=%d)", __FILE__, __LINE__, charId);
        return 0;
    }

    return 0;
}
