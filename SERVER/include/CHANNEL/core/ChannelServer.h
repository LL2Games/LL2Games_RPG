#pragma once

#include "CHANNEL/core/ChannelSession.h"
#include "CHANNEL/core/PlayerManager.h"
#include "CHANNEL/core/MapManager.h"
#include "CHANNEL/db/MySqlConnectionPool.h"
#include "CHANNEL/db/RedisClient.h"
#include "CHANNEL/core/common.h"


class ChannelServer
{
public:
    ChannelServer();
    ~ChannelServer();

    void Init();
    void Run();
    void GameLoop();
    void OnAccept();
    void OnReceive(int fd, char* buf);
    void OnDisconnect(int fd);
    void BroadCast(); // 매개변수로 packet 받아야함
    void SendHeartbeatToWorld();

private:
    int m_channel_id;
    int m_listen_fd;
    std::map<int,ChannelSession*> m_sessions;
    PlayerManager m_player_mamager;
    MySqlConnectionPool m_db;
    RedisClient m_redis;
    MapManager m_map_manager;
    

};
