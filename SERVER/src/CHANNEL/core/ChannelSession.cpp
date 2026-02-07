#include "ChannelSession.h"
#include "ChannelServer.h"
#include "PacketParser.h"
#include "IPacketFactory.h"
#include "Packet.h"

ChannelSession::ChannelSession(int fd, ChannelServer* server) : m_fd(fd), m_server(server)
{
    m_recvBuf.reserve(8192);
}

ChannelSession::~ChannelSession()
{
}


bool ChannelSession::OnBytes(const uint8_t* data, size_t len)
{
    m_recvBuf.insert(m_recvBuf.end(), data, data+len);

    //std::vector<char> buf(m_recvBuf.begin(), m_recvBuf.end());
    auto pkt = PacketParser::Parse(m_recvBuf);
    if (!pkt.has_value())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Packet Parse failed", __FUNCTION__, __LINE__);
        return false;
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] parsed packet type=%x, payload len=%d", __FUNCTION__, __LINE__, pkt->type, (int)pkt->payload.size());


    auto handler = m_factory.Create(pkt->type);
    if(handler)
    {
        PacketContext ctx;
        ctx.type = pkt->type;
        ctx.channel_session = this;
        ctx.fd = m_fd;
        ctx.pkt.type = pkt->type;
        ctx.payload = const_cast<char*>(pkt->payload.c_str());
        ctx.payload_len = pkt->payload.size();
        
        // PlayerManager 설정
        if (m_server) {
            ctx.player_manager = m_server->GetPlayerManager();
            K_slog_trace(K_SLOG_DEBUG, "Player_Manager [%p]\n", ctx.player_manager);
            ctx.map_service = m_server->GetMapService();
            ctx.player_service = m_server->GetPlayerService();
            ctx.stat_service = m_server->GetStatService();
        }
        
        handler->Execute(&ctx);
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

//[L][V] [L][V] [L][V]

//클라입력 $  [L][V]
int ChannelSession::Send(int type, const std::vector<std::string>& payload)
{
    std::string body = PacketParser::MakeBody(payload);
    std::string packet = PacketParser::MakePacket(type, body);
    send(m_fd, packet.c_str(), packet.size(), 0);

    return 0;
}

int ChannelSession::SendOk(int type, std::vector<std::string> payload)
{
    std::vector<std::string>::iterator payloadIter;

    for(payloadIter = payload.begin(); payloadIter < payload.end(); ++payloadIter)
    {
         K_slog_trace(K_SLOG_TRACE, "[%s][%d] body[%s]", __FUNCTION__, __LINE__, payloadIter->c_str());
    }
    payload.insert(payload.begin(), "ok");
    std::string body = PacketParser::MakeBody(payload);
    std::string packet = PacketParser::MakePacket(type, body);
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] body[%s]", __FUNCTION__, __LINE__, body.c_str());
    send(m_fd, packet.c_str(), packet.size(), 0);
    return 0;
}

int ChannelSession::SendNok(int type, const std::string &errMsg)
{
    std::vector<std::string> msg;
    msg.push_back("nok");
    msg.push_back(errMsg);
    std::string body = PacketParser::MakeBody(msg);
    std::string packet = PacketParser::MakePacket(type, body);
    send(m_fd, packet.c_str(), packet.size(), 0);
    return 0;
}