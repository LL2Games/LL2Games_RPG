#pragma once
#include "CHANNEL/core/common.h"
#include <queue>

class MySqlConnectionPool
{
public:
    static MySqlConnectionPool *GetInstance();

    int Init(const int poot_size);
    MYSQL* GetConnection();
    int ReleaseConnection(MYSQL*);

private:
    std::queue<MYSQL*> m_pool;
    MySqlConnectionPool(const int pool_size);
    ~MySqlConnectionPool();

    static MySqlConnectionPool* m_instance;
};
