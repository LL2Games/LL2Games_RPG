#include "db/MySQLManager.h"
#include "K_slog.h"

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
    bool rc = false;
    MYSQL_RES* res = nullptr;
    MYSQL_ROW row = nullptr;
    char query[256] = {0};

    snprintf(query, sizeof(query), "SELECT pw FROM users WHERE id = '%s'", id.c_str());

    if (mysql_query(m_conn, query))
        goto err;
    
    res = mysql_store_result(m_conn);
    if (!res) goto err;

    row = mysql_fetch_row(res);
    if (!row)
        goto err;

    rc = (pw == row[0]);

err:
    if(res) mysql_free_result(res);
    return rc;
}

std::string MySQLManager::GetNick(const std::string &id)
{
    std::string rs;
    char query[256] = {0};
    MYSQL_RES* res = nullptr;
    MYSQL_ROW row = nullptr;

    snprintf(query, sizeof(query), "SELECT nick from users WHERE id='%s'", id.c_str());

    if (mysql_query(m_conn, query))
        goto err;
    
    res = mysql_store_result(m_conn);
    if (!res) goto err;

    row = mysql_fetch_row(res);
    if (!row) goto err;

    rs = row[0];

err:
    if (res) mysql_free_result(res);
    return rs;
}