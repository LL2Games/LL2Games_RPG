#include "CHANNEL/core/ChannelSession.h"

ChannelSession::ChannelSession(int fd) : m_fd(fd)
{
    m_recvBuf.reserve(8192);
}

ChannelSession::~ChannelSession()
{
}


bool ChannelSession::OnBytes(const uint8_t* data, size_t len)
{
    m_recvBuf.insert(m_recvBuf.end(), data, data+len);

    while(true)
    {
        Packet pkt; 
        if(!TryPopOnePacket(pkt))
            break;

        HandlePacket(pkt);
    }

    return true;
}


bool ChannelSession::TryPopOnePacket(Packet& outPkt)
{
    if(m_recvBuf.size() < kHeaderSize)
        return false;

    PacketHeader netHeader{};
    memcpy(&netHeader, m_recvBuf.data(), kHeaderSize);

    const uint16_t totalLen = ntohs(netHeader.length);
    const uint16_t type     = ntohs(netHeader.type);

    if(totalLen < kHeaderSize || totalLen > kMaxPacketSize)
    {
       return false; 
    }

    if(m_recvBuf.size() < totalLen)
        return false;

    outPkt.header.length = totalLen;
    outPkt.header.type = type;

    const size_t payloadLen = totalLen - kHeaderSize;
    outPkt.payload.resize(payloadLen);

    if(payloadLen > 0)
    {
        memcpy(outPkt.payload.data(), m_recvBuf.data() + kHeaderSize, payloadLen);

    }

    m_recvBuf.erase(m_recvBuf.begin(), m_recvBuf.begin() + totalLen);
    return true;

}


void ChannelSession::HandlePacket(const Packet& pkt)
{
    // header type에 따라서 스위치 문을 통해 특정 함수가 실행되도옥 구성 예정
    (void)pkt;

}


bool ChannelSession::EnqueueSend(uint16_t type, const void* payload, uint16_t payloadLen)
{
    Packet pkt;
    pkt.header.type = type;
    pkt.payload.resize(payloadLen);

    if(payloadLen > 0)
    {
        memcpy(pkt.payload.data(), payload, payloadLen);
    }

    return EnqueueSend(pkt);
}

bool ChannelSession::EnqueueSend(const Packet& pkt)
{
    const size_t totalLen = kHeaderSize + pkt.payload.size();
    if(totalLen > kMaxPacketSize)
        return false;

    PacketHeader netHeader{};
    netHeader.length = htons(static_cast<uint16_t>(totalLen));
    netHeader.type    = htons(pkt.header.type);

    std::vector<uint8_t> bytes(totalLen);
    memcpy(bytes.data(), &netHeader, kHeaderSize);

    if(!pkt.payload.empty())
    {
        memcpy(bytes.data() + kHeaderSize, pkt.payload.data(), pkt.payload.size());

    }

    m_sendQueue.emplace_back(std::move(bytes));
    return true;
}

bool ChannelSession::FlushSend()
{
    while(!m_sendQueue.empty())
    {
        std::vector<uint8_t>& front = m_sendQueue.front();
        const uint8_t* p =front.data() + m_sendOffset;
        const size_t remain = front.size() - m_sendOffset;

        ssize_t sent = send(m_fd, p, remain,0);
        if(sent < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return true;
            }

            return false;
        }

        if(sent == 0)
        {
            return false;
        }

        m_sendOffset += static_cast<size_t>(sent);

        if(m_sendOffset == front.size())
        {
            m_sendQueue.pop_front();
            m_sendOffset = 0;
        }
    }

    return true;
}

// 지금 방식은 클라이언트 하나에 해당해서 Send를 하는 방식인데 Player 클래스를 vector로 가지고 있고
// 같은 맵, 시야 범위 등등 환경요소들을 확인해서 보내는 방식으로 변경 필요

