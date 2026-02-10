#include "MySqlConnectionPool.h"
#include "K_slog.h"

#define POOL_SIZE 8
#define HOST "127.0.0.1"
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

MYSQL* MySqlConnectionPool::GetConnection()
{
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