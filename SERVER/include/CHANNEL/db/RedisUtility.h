#include <iostream>
#include <string>
#include <map>

#include "../util/PlayerData.h"
#include "CharacterStat.h"
using Map = std::map<std::string, std::string>;



bool GetStr(const Map& map, const std::string& key, std::string& value);
bool GetInt(const Map& map, const std::string& key, int& value);
bool GetFloat(const Map& map, const std::string& key, float& value);

std::map<std::string, std::string> PlayerInfoToRedisMap(const PlayerInitData playerData, const CharacterStat stat);
