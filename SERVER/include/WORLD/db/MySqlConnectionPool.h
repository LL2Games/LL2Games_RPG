#pragma once
#include <mysql/mysql.h>
#include <string>
#include <queue>
#include "ConfigLoader.h"
//SingleTone
class MySqlConnectionPool
{
public:
    static MySqlConnectionPool *GetInstance();
    static MySqlConnectionPool *GetInstance(const MySqlConfig& mysqlConfig, const int pool_size);

    int Init(const int pool_size);
    int Init(const MySqlConfig& mysqlConfig, const int pool_size);
    MYSQL*  GetConnection();
    int ReleaseConnection(MYSQL*);
private:
    std::queue<MYSQL*> m_pool;
    MySqlConnectionPool(const int pool_size);
    MySqlConnectionPool(const MySqlConfig& mysqlConfig, const int pool_size);
    ~MySqlConnectionPool();
    static MySqlConnectionPool *m_instance;
};