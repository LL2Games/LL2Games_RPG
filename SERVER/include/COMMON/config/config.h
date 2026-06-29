#pragma once
#include "common.h"

struct MySqlConfig
{
    std::string host;
    int port;
    std::string user;
    std::string password;
    std::string database;
};


struct RedisConfig
{
    std::string host;
    int port;
};

struct CommonConfig
{
    int logLevel;
};

struct ServerConfig
{
    int port;
    int threadCount;
    int maxUserCount;
};

struct AppConfig
{
    CommonConfig common;
    MySqlConfig mysql;
    RedisConfig redis;

    ServerConfig loginServer;
    ServerConfig worldServer;
    ServerConfig channelServer;
    ServerConfig chatServer;
};