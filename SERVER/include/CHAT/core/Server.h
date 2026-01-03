#pragma once
#include <vector>
#include <sys/select.h>
#include "Client.h"
#include "ChatPacketFactory.h"

class Server {
    public:
    bool Init(const int port);
    void Run();
    
    private:
    int m_listenFd;
    std::vector<Client *> m_clients;
    ChatPacketFactory m_factory;

    void AcceptNewClient();
    void ProcessClient(Client* cli);
    void BroadCast(const std::string& nick, const std::string& msg, const int exceptFd = -1);
};