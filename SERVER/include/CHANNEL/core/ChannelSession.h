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

    bool OnBytes(const uint8_t* data, size_t len);

    bool EnqueueSend(uint16_t type, const void* payload, uint16_t payloadLen);
    bool EnqueueSend(const Packet& pkt);

    bool FlushSend();

    bool NeadWriteEvent() const {return !m_sendQueue.empty();}

    int Send(int type, const std::vector<std::string>& payload);
    int SendOk(int type, std::vector<std::string> payload={});
    int SendNok(int type, const std::string &errMsg);

public: 
    int OnPacket(Packet packet);
    int Send(int type ,std::string data);

    void SetPlayer(Player* player) {m_player = player;}
    Player* GetPlayer() const {return m_player;}

    void SetPlayerManager(PlayerManager* playerManager) {m_playerManager = playerManager;}
    int GetFd() const {return m_fd;}

private:
    void HandlePacket(const Packet& pkt);

    static constexpr size_t kHeaderSize = sizeof(PacketHeader);
    static constexpr size_t kMaxPacketSize = 64 * 1024;


private:
    int m_fd = -1;
    ChannelServer* m_server = nullptr;

    std::vector<char> m_recvBuf;

    std::deque<std::vector<uint8_t>> m_sendQueue;

    size_t m_sendOffset = 0;

    Player* m_player;
    PlayerManager* m_playerManager;
    ChannelPacketFactory m_factory;
};
