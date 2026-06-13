#include "MySqlConnectionPool.h"

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
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] TEST", __FUNCTION__, __LINE__);
        if (!conn)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] mysql_init ERROR", __FUNCTION__, __LINE__);
            continue;
        }
   

        mysql_options(conn, MYSQL_READ_DEFAULT_FILE, "/home/ljh/.my.cnf");
        mysql_options(conn, MYSQL_READ_DEFAULT_GROUP, "CHANNEL");

        if(!conn)
        {
             K_slog_trace(K_SLOG_ERROR, "[%s][%d] mysql_init ERROR", __FUNCTION__, __LINE__);
             continue;
        }
        result = mysql_real_connect(conn, "100.114.42.54", "dyddlswogh","dyddlswogh","game", 3306, nullptr, 0);
    
        if(result == nullptr)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] myslq_real_connect failed : %s", __FUNCTION__, __LINE__, mysql_error(conn));
            mysql_close(conn);
            continue;
        }

        m_pool.push(conn);
       cnt++;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] db pool created[%d]", __FUNCTION__, __LINE__, cnt);
    return 0;
}

int MySqlConnectionPool::Init(const MySqlConfig& mysqlConfig, const int pool_size)
{
    int cnt = 0;

    for (int i = 0; i < pool_size; i++)
    {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] mysql_init ERROR", __FUNCTION__, __LINE__);
            continue;
        }

        MYSQL* result = mysql_real_connect(
            conn,
            mysqlConfig.host.c_str(),
            mysqlConfig.user.c_str(),
            mysqlConfig.password.c_str(),
            mysqlConfig.database.c_str(),
            mysqlConfig.port,
            nullptr,
            0
        );

        if (result == nullptr)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] mysql_real_connect failed : %s",
                         __FUNCTION__, __LINE__, mysql_error(conn));
            mysql_close(conn);
            continue;
        }


        m_pool.push(conn);
        cnt++;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] db pool created[%d]", __FUNCTION__, __LINE__, cnt);
    return 0;
}

MySqlConnectionPool::MySqlConnectionPool(const int pool_size)
{
    Init(pool_size);   
}

MySqlConnectionPool::MySqlConnectionPool(const MySqlConfig& mysqlConfig,  const int pool_size)
{
    Init(mysqlConfig, pool_size);  
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

MySqlConnectionPool* MySqlConnectionPool::GetInstance(const MySqlConfig& mysqlConfig)
{
    if(m_instance == nullptr)
    {
        m_instance = new MySqlConnectionPool(mysqlConfig, POOL_SIZE);
    }

    return m_instance;
}

MYSQL* MySqlConnectionPool::GetConnection()
{
    // m_pool이 생성안됐을 때 대비해서 안전 코드 생성
    if(m_pool.empty())
    {
        return nullptr;
    }
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
