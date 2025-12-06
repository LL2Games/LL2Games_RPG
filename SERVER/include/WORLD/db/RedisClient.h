#pragma once
#include <hiredis/hiredis.h>
#include <string>
#include <map>

//SingleTone
class RedisClient
{
public:
    std::string Get(std::string key);
    int Set(const std::string key, const std::string value);
    std::map<std::string, std::string> HGetAll(const std::string key);
    //Publish(channel, msg)
private:
    redisContext* m_ctx;
};