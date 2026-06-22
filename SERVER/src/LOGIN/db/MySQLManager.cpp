#include "MySQLManager.h"
#include "K_slog.h"
#include <cstring>

MySQLManager& MySQLManager::Instance()
{
    static MySQLManager inst;
    return inst;
}

bool MySQLManager::Connect(const char *host, const char *user, const char *pass, const char *db, const int port)
{
    m_conn = mysql_init(nullptr);
    if (!m_conn)
        return false;

    if (!mysql_real_connect(m_conn, host, user, pass, db, port, nullptr, 0))
        return false;
    
    K_slog_trace(K_SLOG_TRACE, "DB connected to %s OK\n", db);

    return true;
}

bool MySQLManager::Login(const std::string &id, const std::string& pw)
{
     MYSQL_STMT* stmt = mysql_stmt_init(m_conn);
    if(!stmt)
    {
        K_slog_trace(K_SLOG_ERROR, "mysql_stmt_init ERROR [%s]", mysql_error(m_conn));
        return false;
    }

    const char* query = "SELECT pw FROM users WHERE account_id =? ";
    
    if(mysql_stmt_prepare(stmt, query, strlen(query)) !=0)
    {
        K_slog_trace(K_SLOG_ERROR, "mysql_stmt_prepare ERROR [%s]", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    unsigned long idLength = id.size();

    MYSQL_BIND param[1]{};
    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = const_cast<char*>(id.c_str());
    param[0].buffer_length = id.size();
    param[0].length = &idLength;

    if(mysql_stmt_bind_param(stmt, param) != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "mysql_stmt_bind_param ERROR [%s]", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    if(mysql_stmt_execute(stmt) !=0)
    {
        K_slog_trace(K_SLOG_ERROR, "mysql_stmt_execute ERROR [%s]", mysql_stmt_error(stmt));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query);

        mysql_stmt_close(stmt);
        return false;

    }

    char pwBuffer[64]{};
    unsigned long pwLength = 0;
    bool pwIsNull = false;

    MYSQL_BIND resultBind[1]{};

    resultBind[0].buffer_type = MYSQL_TYPE_STRING;
    resultBind[0].buffer = pwBuffer;
    resultBind[0].buffer_length = sizeof(pwBuffer);
    resultBind[0].length = &pwLength;
    resultBind[0].is_null = &pwIsNull;

    if(mysql_stmt_bind_result(stmt, resultBind) != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "mysql_stmt_bind_result ERROR [%s]", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    if(mysql_stmt_store_result(stmt) != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "mysql_stmt_store_result ERROR [%s]", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;

    }

    if (!pwIsNull)
    {
       if(pwBuffer == pw)
            return true;
    }
    else
    {
        return false;
    }

    return true;
}
