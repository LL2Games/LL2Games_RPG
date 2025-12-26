#include "CHANNEL/db/MySqlConnectionPool.h"

#define POOL_SIZE 8

MySqlConnectionPool *MySqlConnectionPool::m_instance=nullptr;

int MySqlConnectionPool::Init(const int pool_size)
{
    int cnt = 0;
    
    for(int i =0; i < pool_size; i++)
    {
        MYSQL* conn = nullptr;
        MYSQL* result;
        conn = mysql_init(nullptr);

        mysql_options(conn, MYSQL_READ_DEFAULT_GROUP, "CHANNEL");

        if(!conn)
        {
             K_slog_trace(K_SLOG_ERROR, "[%s][%d] mysql_init ERROR", __FUNCTION__, __LINE__);
             continue;
        }
                                    //host, user, passwd, db, port, unix_socket, clientflag
        result = mysql_real_connect(conn, nullptr, nullptr, nullptr, nullptr, 0, nullptr,0);
        if(result == nullptr)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] myslq_real_connect failed : %s", __FUNCTION__, __LINE__, mysql_error(conn));
            mysql_close(conn);
            continue;
        }
        m_pool.push(conn);
       cnt++;
    }

    K_slog_trace(K_SLOG_ERROR, "[%s][%d] db pool created[%d]", __FUNCTION__, __LINE__, cnt);
    return 0;
}

MySqlConnectionPool::MySqlConnectionPool(const int pool_size)
{
    Init(pool_size);   
}

MySqlConnectionPool::~MySqlConnectionPool()
{
    if(m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

MySqlConnectionPool* MySqlConnectionPool::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new MySqlConnectionPool(POOL_SIZE);
    }

    return m_instance;
}

MYSQL* MySqlConnectionPool::GetConnection()
{
    MYSQL* conn = m_pool.front();
    m_pool.pop();
    return conn;
}

int MySqlConnectionPool::ReleaseConnection(MYSQL* conn)
{
    if(conn !=nullptr)
    {
        m_pool.push(conn);
    }
    return 0;
}
