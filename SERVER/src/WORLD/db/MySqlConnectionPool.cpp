#include "MySqlConnectionPool.h"
#include "K_slog.h"

#define POOL_SIZE 8
#define HOST "100.114.42.54"
#define USER "root"
#define PASSWD "1234"
#define DB "game"
#define PORT 3306

MySqlConnectionPool *MySqlConnectionPool::m_instance = nullptr;

int MySqlConnectionPool::Init(const int pool_size)
{
    int cnt = 0;
    //db커넥션 반복문으로 생성
    for (int i = 0; i < pool_size; i++)
    {
        MYSQL* conn = nullptr;
        conn = mysql_init(nullptr);
        if (!conn)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] mysql_init", __FUNCTION__, __LINE__);
            continue;
        }
        if (!mysql_real_connect(conn, HOST, USER, PASSWD, DB, PORT, nullptr, 0))
        {
             K_slog_trace(K_SLOG_ERROR,
                "[%s][%d] mysql_real_connect failed: %s",
                __FUNCTION__, __LINE__, mysql_error(conn));

            mysql_close(conn);
            continue;
        }
        mysql_autocommit(conn, 0);
        m_pool.push(conn);
        cnt++;
    }

    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] db pool created[%d]", __FUNCTION__, __LINE__, cnt);
    return 0;
}

int MySqlConnectionPool::Init(const MySqlConfig& mysqlConfig, const int pool_size)
{
    int cnt = 0;
    //db커넥션 반복문으로 생성
    for (int i = 0; i < pool_size; i++)
    {
        MYSQL* conn = nullptr;
        conn = mysql_init(nullptr);
        if (!conn)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] mysql_init", __FUNCTION__, __LINE__);
            continue;
        }
        if (!mysql_real_connect(conn, mysqlConfig.host.c_str(), mysqlConfig.user.c_str(), mysqlConfig.password.c_str(), mysqlConfig.database.c_str(), mysqlConfig.port, nullptr, 0))
        {
             K_slog_trace(K_SLOG_ERROR,
                "[%s][%d] mysql_real_connect failed: %s",
                __FUNCTION__, __LINE__, mysql_error(conn));

            mysql_close(conn);
            continue;
        }
        mysql_autocommit(conn, 0);
        m_pool.push(conn);
        cnt++;
    }

    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] db pool created[%d]", __FUNCTION__, __LINE__, cnt);
    return 0;
}

MySqlConnectionPool::MySqlConnectionPool(const int pool_size)
{
    Init(pool_size);
    
}

MySqlConnectionPool::MySqlConnectionPool(const MySqlConfig& mysqlConfig, const int pool_size)  
{
    Init(mysqlConfig, pool_size);
}


MySqlConnectionPool::~MySqlConnectionPool()
{
    if (m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

MySqlConnectionPool* MySqlConnectionPool::GetInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = new MySqlConnectionPool(POOL_SIZE);
    }

    return m_instance;
}

MySqlConnectionPool* MySqlConnectionPool::GetInstance(const MySqlConfig& mysqlConfig, const int pool_size)
{
    if(m_instance == nullptr)
    {
        m_instance = new MySqlConnectionPool(mysqlConfig, pool_size);
    }
    
    return m_instance;
}

MYSQL* MySqlConnectionPool::GetConnection()
{
    // m_pool 생성 안됐을 때 대비 안전 코드 생성
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
    if (conn != nullptr)
    {
        m_pool.push(conn);
    }
    return 0;
}