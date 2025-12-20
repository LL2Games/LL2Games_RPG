#pragma once
#include <mysql/mysql.h>
#include <string>
#include <queue>

//SingleTone
class MySqlConnectionPool
{
public:
    static MySqlConnectionPool *GetInstance();

    int Init(const int pool_size);
    MYSQL*  GetConnection();
    int ReleaseConnection(MYSQL*);
private:
    std::queue<MYSQL*> m_pool;
    MySqlConnectionPool(const int pool_size);
    ~MySqlConnectionPool();
    static MySqlConnectionPool *m_instance;
};