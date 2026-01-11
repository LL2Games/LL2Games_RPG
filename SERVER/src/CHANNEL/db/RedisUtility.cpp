#include "CHANNEL/db/RedisUtility.h"
#include "CHANNEL/util/PlayerData.h"
#include <sstream>

bool GetStr(const Map& map, const std::string& key, std::string& value)
{
    auto it = map.find(key);
    if(it == map.end()) return false;
    value = it->second;
    return true;
}

bool GetInt(const Map& map, const std::string& key, int& value)
{
    auto it = map.find(key);
    if(it == map.end()) return false;
    try{
        value = std::stoi(it->second);
    }catch(...){
        return false;
    }
    return true; 
}

bool GetFloat(const Map& map, const std::string& key, float& value)
{
    auto it = map.find(key);
    if(it == map.end()) return false;
    try{
        value = std::stof(it->second);
    }catch(...){
        return false;
    }

    return true;
}

std::map<std::string, std::string> PlayerInfoToRedisMap(const PlayerInitData playerData)
{
    std::map<std::string, std::string> redisMap;
    
    redisMap["char_id"] = std::to_string(playerData.char_id);
    redisMap["account_id"] = playerData.account_id;
    redisMap["name"] = playerData.name;
    redisMap["level"] = std::to_string(playerData.level);
    redisMap["job"] = std::to_string(playerData.job);
    redisMap["map_id"] = std::to_string(playerData.map_id);
    redisMap["xPos"] = std::to_string(playerData.xPos);
    redisMap["yPos"] = std::to_string(playerData.yPos);
    
    return redisMap;
}

