#pragma once
#include "CHANNEL/core/common.h"
#include "CHANNEL/core/Player.h"


class ChannelSession
{
public:
    explicit ChannelSession(int fd);
    ~ChannelSession();


    int GetFd() const {return m_fd;}

    bool OnBytes(const uint8_t* data, size_t len);

    bool EnqueueSend(uint16_t type, const void* payload, uint16_t payloadLen);
    bool EnqueueSend(const Packet& pkt);

    bool FlushSend();

    bool NeadWriteEvent() const {return !m_sendQueue.empty();}

public: 
    int OnPacket(Packet packet);
    int Send(int type ,std::string data);


private:
    bool TryPopOnePacket(Packet& outpkt);
    void HandlePacket(const Packet& pkt);

    static constexpr size_t kHeaderSize = sizeof(PacketHeader);
    static constexpr size_t kMaxPacketSize = 64 * 1024;


private:
    int m_fd = -1;

    std::vector<uint8_t> m_recvBuf;

    std::deque<std::vector<uint8_t>> m_sendQueue;

    size_t m_sendOffset = 0;

    Player* m_player;

};
