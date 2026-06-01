#pragma once
#include "common.h"
#include "Player.h"
#include "PlayerManager.h"
#include "ChannelPacketFactory.h"

class ChannelServer;

class ChannelSession
{
public:
    explicit ChannelSession(int fd, ChannelServer* server = nullptr);
    ~ChannelSession();


    int OnPacket(Packet packet);
    bool OnBytes(const uint8_t* data, size_t len);

    int Send(int type, const std::vector<std::string>& payload);
    //int Send(int type, std::vector<std::string> payload={});
    int SendOk(int type, std::vector<std::string> payload={});
    int SendNok(int type, const std::string &errMsg);

public: 
    void SetPlayer(Player* player) {m_player = player;}
    Player* GetPlayer() const {return m_player;}

    void SetPlayerManager(PlayerManager* playerManager) {m_playerManager = playerManager;}
    int GetFd() const {return m_fd;}

private:
   
private:
    int m_fd = -1;
    ChannelServer* m_server = nullptr;

    std::vector<char> m_recvBuf;

    Player* m_player;
    PlayerManager* m_playerManager;
    ChannelPacketFactory m_factory;
};
