#pragma once
#include <hiredis/hiredis.h>
#include <string>
#include <map>
#include <optional>

enum TTL {
    E_TTL_CHARLIST = 300,
};

//SingleTone
class RedisClient
{
public:
    RedisClient();
    ~RedisClient();

    static RedisClient *GetInstance();

    int Set(const std::string key, const std::string value);
    std::string Get(std::string key);
    
    int HSet(const std::string key, const std::string& field, const std::string &value, const int expire = 60);
    std::optional<std::map<std::string, std::string>> HGetAll(const std::string key);
    //Publish(channel, msg)
private:
    redisContext* m_ctx;
    static RedisClient *m_instance;

    int Init();
};