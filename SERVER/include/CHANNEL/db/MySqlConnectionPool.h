#pragma once
#include "CHANNEL/core/common.h"
#include <queue>
#include "ConfigLoader.h"

class MySqlConnectionPool
{
public:
    static MySqlConnectionPool *GetInstance();
    static MySqlConnectionPool *GetInstance(const MySqlConfig& mysqlConfig);

    int Init(const int poot_size);
    int Init(const MySqlConfig& mysqlConfig, const int pool_size);
    
    MYSQL* GetConnection();
    int ReleaseConnection(MYSQL*);

private:
    std::queue<MYSQL*> m_pool;
    MySqlConnectionPool(const int pool_size);
    MySqlConnectionPool(const MySqlConfig& mysqlConfig,  const int pool_size);
    ~MySqlConnectionPool();

    static MySqlConnectionPool* m_instance;
};
