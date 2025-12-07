#pragma once
#include <string>
#include <map>
#include "MySqlConnectionPool.h"
#include "RedisClient.h"
#include "WorldSession.h"
#include "ChannelManager.h"

class WorldServer
{
public:
    WorldServer();
    ~WorldServer();
    int Init(const std::string& configPath);
    int Init(const int port);
    int Run();
    int OnAccept();
    int OnReceive(int fd, const std::string& buf);
    int OnReceive(int fd);
    int OnDisconnect(int fd);
    int HandleSelectCharacter(int fd, const std::string& charId);
    int HandleChannelHeartBeat(const std::string& pkt);

private:
    int m_listen_fd;
    std::map<int, WorldSession*> m_sessions;
    ChannelManager  m_channel_manager;
    RedisClient m_redis;
};