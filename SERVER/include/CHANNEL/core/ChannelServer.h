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

    bool Init(const int port);
    void Run();

    void OnReceive(int fd, char* buf, size_t len);
    void OnDisconnect(int fd);
    void BroadCast(); // 매개변수로 packet 받아야함
    void SendHeartbeatToWorld();
   
private:
    bool InitListenSocket(int port);
    bool InitEpoll();

    void GameLoop();
    void OnAccept();

    static int SerNonblocking(int fd);

private:
    int m_channel_id;
    int m_listen_fd;
    int m_epfd;
    bool m_running;

    std::vector<epoll_event> m_events;
    std::unordered_map<int,ChannelSession*> m_sessions;
    PlayerManager m_player_mamager;
    //MySqlConnectionPool m_db;
    RedisClient m_redis;
    MapManager m_map_manager;
    

};
