#include "ChannelSession.h"
#include "ChannelServer.h"
#include "PacketParser.h"
#include "IPacketFactory.h"
#include "Packet.h"
#include "MapInstance.h"
#include "PlayerManager.h"

ChannelSession::ChannelSession(int fd, ChannelServer* server) : m_fd(fd), m_server(server), m_player(nullptr), m_playerManager(nullptr)
{
   m_recvBuf.reserve(8192);
}

ChannelSession::~ChannelSession()
{
    if (m_player != nullptr)
    {
        MapInstance *map = m_player->GetCurrentMap();
        
        if (map != nullptr)
        {
            //플레이어 접속 종료시 해당맵에서 퇴장
            map->OnLeave(m_player->GetId());
            K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] map->OnLeave(Id:%d)", __FILE__, __FUNCTION__, __LINE__, m_player->GetId());
        }

        if (m_playerManager != nullptr)
        {
            m_playerManager->RemovePlayer(m_player->GetId());
        }
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s:%s][%d] ChannelSession Destroy(fd:%d)", __FILE__, __FUNCTION__, __LINE__, m_fd);
}


bool ChannelSession::OnBytes(const uint8_t* data, size_t len)
{
    m_recvBuf.insert(m_recvBuf.end(), data, data+len);

    while (true)
    {
        ParseResult result = PacketParser::TryParse(m_recvBuf);

        if (result.status == ParseStatus::NeedMoreData)
        {
            return true;
        }

        if (result.status == ParseStatus::InvalidPacket)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] Invalid packet", __FUNCTION__, __LINE__);
            return false;
        }

        Dispatch(result.packet);
    }
    return true;
}

void ChannelSession::Dispatch(const ParsedPacket &pkt)
{
    auto handler = m_factory.Create(pkt.type);
    if(handler)
    {
        PacketContext ctx;
        ctx.type = pkt.type;
        ctx.channel_session = this;
        ctx.fd = m_fd;
        ctx.type = pkt.type;
        ctx.payload = const_cast<char*>(pkt.payload.c_str());
        ctx.payload_len = pkt.payload.size();
        
        // PlayerManager 설정
        if (m_server) {
            ctx.player_manager = m_server->GetPlayerManager();
            //K_slog_trace(K_SLOG_DEBUG, "Player_Manager [%p]\n", ctx.player_manager);
            ctx.map_service = m_server->GetMapService();
            ctx.player_service = m_server->GetPlayerService();
            ctx.stat_service = m_server->GetStatService();
            ctx.item_service = m_server->GetItemService();
            ctx.combat_service = m_server->GetCombatService();
            ctx.trade_service = m_server->GetTradeService();
        }
        
        handler->Execute(&ctx);
    }
}
// 지금 방식은 클라이언트 하나에 해당해서 Send를 하는 방식인데 Player 클래스를 vector로 가지고 있고
// 같은 맵, 시야 범위 등등 환경요소들을 확인해서 보내는 방식으로 변경 필요

//[L][V] [L][V] [L][V]

//클라입력 $  [L][V]
int ChannelSession::Send(int type, const std::vector<std::string>& payload)
{
    std::string body = PacketParser::MakeBody(payload);
    std::string packet = PacketParser::MakePacket(type, body);
    
    return EnqueueSend(std::move(packet));
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

   return EnqueueSend(std::move(packet));
}

int ChannelSession::SendNok(int type, const std::string &errMsg)
{
    std::vector<std::string> msg;
    msg.push_back("nok");
    msg.push_back(errMsg);
    std::string body = PacketParser::MakeBody(msg);
    std::string packet = PacketParser::MakePacket(type, body);

    return EnqueueSend(std::move(packet));
}

int ChannelSession::EnqueueSend(std::string packet)
{
    if (packet.empty())
        return -1;

    m_sendQueue.push_back(std::move(packet));

    if (m_server != nullptr)
        m_server->EnableWriteEvent(m_fd);

    return 0;
}

bool ChannelSession::FlushSend()
{
    while (!m_sendQueue.empty())
    {
        std::string& packet = m_sendQueue.front();

        ssize_t sent = send(
            m_fd,
            packet.data() + m_sendOffset,
            packet.size() - m_sendOffset,
            MSG_NOSIGNAL
        );

        if (sent > 0)
        {
            m_sendOffset += static_cast<size_t>(sent);

            if (m_sendOffset == packet.size())
            {
                m_sendQueue.pop_front();
                m_sendOffset = 0;
            }

            continue;
        }

        if (sent < 0)
        {
            if (errno == EINTR)
                continue;

            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return true;

            K_slog_trace(
                K_SLOG_ERROR,
                "[%s : %s : %d] send failed fd:%d errno:%d",
                __FILE__,
                __FUNCTION__,
                __LINE__,
                m_fd,
                errno
            );

            return false;
        }

        return false;
    }

    return true;
}

bool ChannelSession::HasPendingSend() const
{
   return !m_sendQueue.empty();
}

