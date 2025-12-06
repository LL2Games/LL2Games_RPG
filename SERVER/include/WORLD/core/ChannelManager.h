#pragma once
#include <map>
#include <string>
#include "RedisClient.h"

struct ChannelInfo
{
    std::string channel_id;
    std::string ip;
    int port;
    int max_users;
    int current_users;
    int alive;
};

class ChannelManager
{
    int AddOrUpdateChannel(const std::string info);
    ChannelInfo SelectBestChannel();
    ChannelInfo* GetChannel(const std::string& id);
    int LoadFromRedis(RedisClient&);
    int SaveToRedis(RedisClient&);
private:
    std::map<int, ChannelInfo> m_channels;
};