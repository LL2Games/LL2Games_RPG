#include "WorldServer.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "K_slog.h"
#include "COMMON/packet/PacketParser.h"
#include "COMMON/packet/PacketFactory.h"
#include "util/type.h"

WorldServer::WorldServer() {}
WorldServer::~WorldServer() {}

int WorldServer::Init(const std::string &configPath)
{
    //server conf read, parsing
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] configPath[%s]", __FUNCTION__, __LINE__, configPath); //test
    return 0;
}

int WorldServer::Init(const int port)
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] socket", DAEMON_NAME);
        return -1;
    }

    int opt = 1;
    setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(m_listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] bind [port=%d]", DAEMON_NAME, port);
        return false;
    }
    if (listen(m_listen_fd, 10) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] listen", DAEMON_NAME);
        return false;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s] Listening on %d", DAEMON_NAME, port);

    return 0;
}

int WorldServer::Run()
{
    int idx = 0;
    fd_set reads;

    while (true)
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] Run[%d]", __FUNCTION__, __LINE__, ++idx);
        FD_ZERO(&reads);
        FD_SET(m_listen_fd, &reads);

        int fd_max = m_listen_fd;
        for (auto session : m_sessions)
        {
            const int sFd = session.first;
            FD_SET(sFd, &reads);
            if (sFd > fd_max) fd_max = sFd;
        }

        int ret = select(fd_max + 1, &reads, nullptr, nullptr, nullptr);
        if (ret < 0)
        {
            K_slog_trace(K_SLOG_ERROR, "select() error");
            break;
        }

        if (FD_ISSET(m_listen_fd, &reads))
            OnAccept();
        
        for (auto session : m_sessions)
        {
            if (FD_ISSET(session.first, &reads))
                OnReceive(session.first);
        }

    }
    return 0;
}

int WorldServer::OnAccept()
{
    struct sockaddr_in clnt_addr{};
    socklen_t addr_len = sizeof(clnt_addr);

    int client_fd = accept(m_listen_fd, (struct sockaddr *)&clnt_addr, &addr_len);
    if (client_fd < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] accept", __FUNCTION__, __LINE__);
        return -1;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] client_accept[fd=%d]", __FUNCTION__, __LINE__, client_fd);
    WorldSession *session = new WorldSession(client_fd);
    m_sessions[client_fd] = session;

    OnReceive(client_fd);   //첫 ID 입력

    return 0;
}

int WorldServer::OnReceive(int fd)
{
    char temp[BUFFER_SIZE];
    int tempLen = 0;
    std::string buf;
    WorldSession *session = m_sessions[fd];

    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session error", __FUNCTION__, __LINE__);
        return -1;
    }

    do
    {
        memset(temp, 0x00, sizeof(temp));
        tempLen = recv(fd, temp, sizeof(temp), 0);
        if (tempLen <= 0)
        {
            //disconnect
            //OnDisconnect(fd);
            K_slog_trace(K_SLOG_TRACE, "Client %d disconnected\n", fd);
            close(fd);
            delete m_sessions[fd];
            m_sessions.erase(fd);
            return 1;
        }
        buf.append(temp, tempLen);
    } while (tempLen == BUFFER_SIZE);

    session->m_recvBuffer.insert(session->m_recvBuffer.end(), buf.begin(), buf.end());

    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] recv from fd=%d, len=%d", __FUNCTION__, __LINE__, fd, buf.size());
    auto pkt = PacketParser::Parse(session->m_recvBuffer);
    if (!pkt.has_value())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Packet Parse failed", __FUNCTION__, __LINE__);
        return -1;
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] parsed packet type=%x, payload len=%d", __FUNCTION__, __LINE__, pkt->type, (int)pkt->payload.size());

    auto handler = PacketFactory::Create(pkt->type);
    PacketContext ctx;
    ctx.session = m_sessions[fd];
    ctx.char_service = &m_char_service;
    ctx.fd = fd;
    ctx.payload = (char *)pkt->payload.c_str();
    ctx.payload_len = pkt->payload.size();
    if (handler)
    {
        handler->Execute(&ctx);
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] ProcessClient fd=%d done", __FUNCTION__, __LINE__, fd);

    return 0;
}

