#pragma once
#include "CHANNEL/db/RedisUtility.h"

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

