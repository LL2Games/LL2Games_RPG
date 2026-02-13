#pragma once

#include "ChannelSession.h"
#include "PlayerManager.h"
#include "PlayerService.h"
#include "MapService.h"
#include "MapManager.h"
#include "MySqlConnectionPool.h"
#include "RedisClient.h"
#include "common.h"
#include "StatService.h"
#include "ItemManager.h"
#include "ItemService.h"



class ChannelServer
{
public:
    ChannelServer();
    ~ChannelServer();

    bool Init(const int port);
    void Run();

    void OnReceive(int fd);
    void OnDisconnect(int fd);
    void BroadCast(); // 매개변수로 packet 받아야함
    PlayerManager* GetPlayerManager() { return &m_player_mamager; }
    MapService* GetMapService() {return &m_map_service;}
    PlayerService* GetPlayerService() {return &m_player_service;}
    StatService* GetStatService() {return &m_stat_service;}
    ItemService* GetItemService() {return &m_item_service;}
private:
    bool InitListenSocket(int port);
    bool InitEpoll();

    void GameLoop();
    void OnAccept();

    static int SetNonblocking(int fd);

private:
    int m_channel_id;
    int m_listen_fd;
    int m_epfd;
    bool m_running;

    std::vector<epoll_event> m_events;
    std::unordered_map<int,ChannelSession*> m_sessions;
    PlayerManager m_player_mamager;
    PlayerService m_player_service;
    MapManager m_map_manager;
    ItemManager* m_item_manager;
    //MySqlConnectionPool m_db;
    RedisClient m_redis;
    MapService m_map_service;
    StatService m_stat_service;
    ItemService m_item_service;
};
