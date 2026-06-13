#pragma once
#include "common.h"
#include "config.h"
#include <unordered_map>

class ConfigLoader
{
public:
    bool Load(const std::string& path);
    AppConfig ToAppConfig() const;
private:
    std::string GetString(const std::string& section, const std::string& key) const;
    int GetInt(const std::string& section, const std::string& key) const;

    static std::string Trim(const std::string& value);
private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> values_;
};