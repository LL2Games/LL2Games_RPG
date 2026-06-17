#pragma once
#include <mysql/mysql.h>
#include <string>

// Signetone

class MySQLManager
{
public:
    static MySQLManager &Instance();

    bool Connect(const char *host, const char *user, const char *pass, const char *db, const int port);

    bool Login(const std::string &id, const std::string &pw);
    std::string GetNick(const std::string &id);

private:
    MySQLManager() = default;
    MYSQL *m_conn = nullptr;
};