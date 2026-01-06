#pragma once
#include "CHANNEL/core/common.h"
#include <optional>

class RedisClient
{
public:
    RedisClient();
    ~RedisClient();

    static RedisClient *GetInstance();

    int Set(const std::string key, const std::string value);
    std::string Get(std::string key);

    int HSet(const std::string key, const std::string& field, const std::string &value, const int expire =60);
    int HSetAll(const std::string& key, std::map<std::string, std::string> redis_map, const int expire);
    std::optional<std::map<std::string, std::string>> HGetAll(const std::string key);

private:
    redisContext* m_ctx;
    static RedisClient *m_instance;

    int Init();
};
