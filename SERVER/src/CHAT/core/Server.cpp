#include "CHAT/core/Server.h"
#include "CHAT/packet/PacketParser.h"
#include "K_slog.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define DAEMON_NAME "CHAT_DAEMON"
#define BUFFER_SIZE 1024

bool Server::Init(const int port)
{
    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenFd < 0 ) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] socket", DAEMON_NAME);
        return false;
    }

    int opt = 1;
    setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port =htons(port);

    if (bind(m_listenFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] bind [port=%d]", DAEMON_NAME, port);
        return false;
    }
    if (listen(m_listenFd, 10) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] listen", DAEMON_NAME);
        return false;
    }
    
    K_slog_trace(K_SLOG_TRACE, "[%s] Listening on %d", DAEMON_NAME, port);

    return true;
}

void Server::Run()
{
    fd_set reads;

    while (true)
    {
        FD_ZERO(&reads);
        FD_SET(m_listenFd, &reads);

        int fd_max = m_listenFd;

        for (auto c: m_clients)
        {
            FD_SET(c->GetFD(), &reads);
            if (c->GetFD() > fd_max) fd_max = c->GetFD();
        }

        int ret = select(fd_max + 1, &reads, nullptr, nullptr, nullptr);
        if (ret < 0)
        {
            K_slog_trace(K_SLOG_ERROR, "select() error");
            break;
        }

        if (FD_ISSET(m_listenFd, &reads))
            AcceptNewClient();

        for (auto cli : m_clients)
        {
            if(FD_ISSET(cli->GetFD(), &reads))
                ProcessClient(cli);
        }
    }
}

void Server::AcceptNewClient()
{
    struct sockaddr_in caddr {};
    socklen_t len = sizeof(caddr);
    int cfd = accept(m_listenFd, (struct sockaddr*)&caddr, &len);

    if (cfd < 0) return;

    m_clients.push_back(new Client(cfd));

    K_slog_trace(K_SLOG_TRACE, "[%s] Client %d connected", DAEMON_NAME, cfd);
}

void Server::ProcessClient(Client *cli)
{
    char temp[BUFFER_SIZE];
    int  tempLen = 0;
    std::string buf;

    do{
        memset(temp, 0x00, sizeof(temp));
        tempLen = recv(cli->GetFD(), temp, sizeof(temp), 0);
        if (tempLen <= 0)
        {
            //disconnect
            K_slog_trace(K_SLOG_TRACE, "[%s] Client %d disconnected", DAEMON_NAME, cli->GetFD());
            close(cli->GetFD());
            for (auto it = m_clients.begin(); it != m_clients.end(); it++)
            {
                if (*it == cli)
                {
                    delete cli;
                    m_clients.erase(it);
                    break;
                }
            }
            return;
        }
        buf += temp;
    } while (tempLen == BUFFER_SIZE);
    cli->m_recvBuffer.insert(cli->m_recvBuffer.end(), buf.begin(), buf.end());

    PacketParser::Parse(cli, m_clients, 
        [&](const std::string &msg, const int execptFD){
            this->BroadCast(msg, execptFD);
        }
    );
}

void Server::BroadCast(const std::string &msg, const int exceptFd)
{
    for (auto cli : m_clients)
    {
        if (cli->GetFD() == exceptFd)
            continue;
        
        send(cli->GetFD(), msg.c_str(), msg.size(), 0);
    }
}
