#include "core/Server.h"
#include "packet/PacketParser.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "K_slog.h"

bool Server::Init(int port)
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd < 0) return false;

    int opt = 1;
    setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(m_listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        return false;
    
    if (listen(m_listen_fd, 10) < 0)
        return false;
    
    K_slog_trace(K_SLOG_TRACE, "[%s] Listening on port %d\n", "LOGIN", port);

    return true;
}

void Server::Run()
{
    fd_set reads;

    while(true)
    {
        FD_ZERO(&reads);
        FD_SET(m_listen_fd, &reads);
        int fd_max = m_listen_fd;

        for (auto c: m_clients)
        {
            FD_SET(c->GetFD(), &reads);
            if (c->GetFD() > fd_max)
                fd_max = c->GetFD();
        }

        int ret = select(fd_max + 1, &reads, nullptr, nullptr, nullptr);
        if (ret < 0)
        {
            K_slog_trace(K_SLOG_TRACE, "select error\n");
            break;
        }

        //신규 접속
        if(FD_ISSET(m_listen_fd, &reads))
            AcceptNewClient();
        
        //기존 클라이언트 처리
        for (int i = 0; i < (int)m_clients.size(); i++)
        {
            Client* cli = m_clients[i];
            if (FD_ISSET(cli->GetFD(), &reads))
                ProcessClient(cli);
        }
    }
}

void Server::AcceptNewClient()
{
    struct sockaddr_in clnt_addr {};
    socklen_t addr_len = sizeof(clnt_addr);

    int client_fd = accept(m_listen_fd, (struct sockaddr*)&clnt_addr, &addr_len);
    if (client_fd < 0) return;

    m_clients.push_back(new Client(client_fd));

    K_slog_trace(K_SLOG_TRACE, "[%s] Client %d connected\n", "LOGIN", client_fd);
}

void Server::ProcessClient(Client* cli)
{

    char temp[4096];
    int recv_len = recv(cli->GetFD(), temp, sizeof(temp), 0);

    if (recv_len <= 0)
    {
        K_slog_trace(K_SLOG_TRACE, "Client %d disconnected\n", cli->GetFD());
        close(cli->GetFD());

        // 제거
        for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
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

    // 버퍼 누적
    cli->m_recvBuffer.insert(cli->m_recvBuffer.end(), temp, temp + recv_len);

    // 패킷 파싱
    PacketParser::Parse(cli);
}