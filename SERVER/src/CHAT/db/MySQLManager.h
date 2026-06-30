#pragma once
#include <mysql/mysql.h>
#include <string>
#include "ConfigLoader.h"
#include "MySqlConnectionPool.h"
// Signetone

class MySQLManager
{
public:
    static int Init(const MySqlConfig& mysqlConfig);
    static MySQLManager *GetInstance();
    std::string GetNick(const std::string &id);

private:
    explicit MySQLManager();
    static MySQLManager* m_instance;
    MySqlConnectionPool* m_pool;
};