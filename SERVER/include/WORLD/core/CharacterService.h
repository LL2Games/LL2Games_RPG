#pragma once
#include <string>
#include "MySqlConnectionPool.h"
#include "RedisClient.h"

class CharacterService
{
public:
    int GetCharacterList(const std::string& account_id);
    int LoadCharacterSummary(const std::string& char_id);
    int CacheCharacterSummary(const std::string& char_id);
private:
    MySqlConnectionPool& m_db;
    RedisClient& m_redis;
};