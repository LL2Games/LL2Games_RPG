#pragma once
#include <mysql/mysql.h>
#include <string>
#include <queue>

//SingleTone
class MySqlConnectionPool
{
    MYSQL*  GetConnection();
    int ReleaseConnection(MYSQL*);
private:
    std::queue<MYSQL*> m_pool;
};