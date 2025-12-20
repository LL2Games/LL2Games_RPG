#include "CHANNEL/core/ChannelServer.h"
#include "CHANNEL/core/common.h"

ChannelServer::ChannelServer() : m_channel_id(0), m_listen_fd(0), m_epfd(0), m_running(false)
{
    
}

ChannelServer::~ChannelServer()
{

}

int ChannelServer::SerNonblocking(int fd)
{
    // fd의 현재 설정된 값을 가지고 온다.
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    // fd의 현재 설정된 값을 유지하면서 non_block 옵션을 추가로 설정해준다.
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return -1;
    return 0;
}




bool ChannelServer::Init(const int port)
{
   if(!InitListenSocket(port))
   {
        return false;
   }

   if(!InitEpoll())
   {
        return false;
   }

   return true;


}

bool ChannelServer::InitListenSocket(int port)
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(m_listen_fd < 0)
    {
        return false;
    }

    int opt = 1;
    if(setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(m_listen_fd);
        m_listen_fd =-1;
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if(bind(m_listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr) < 0))
    {
        close(m_listen_fd);
        m_listen_fd = -1;
        return false;
    }

    if(listen(m_listen_fd, 1024) < 0)
    {
        close(m_listen_fd);
        m_listen_fd = -1;
        return false;
    }

    if(SerNonblocking(m_listen_fd) < 0)
    {
        close(m_listen_fd);
        m_listen_fd = -1;
        return false;
    }

    std::cout << "Listening on Port" << port << std::endl;
    return true;


}

bool ChannelServer::InitEpoll()
{
    m_epfd = epoll_create1(EPOLL_CLOEXEC);
    if(m_epfd < 0)
    {
        return false;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = m_listen_fd;

    if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listen_fd, &ev) < 0)
    {
        close(m_epfd);
        m_epfd = -1;
        return false;
    }

    m_events.resize(64);
    return true;
}

void ChannelServer::Run()
{
    if(m_listen_fd < 0 || m_epfd < 0)
    {
        std::cout << "Run failed : server not initialized." << std::endl;
        return;
    }

    m_running = true;
    GameLoop();

}

void ChannelServer::GameLoop()
{
    while(true)
    {
        int n = epoll_wait(m_epfd, m_events.data(), static_cast<int>(m_events.size()), -1);

        if( n < 0)
        {
            if(errno == EINTR) continue;
            break;
        }

        for(int i = 0; i < n; ++i)
        {
            int fd = m_events[i].data.fd;
            uint32_t e = m_events[i].events;

            if(fd == m_listen_fd)
            {
                OnAccept();
                continue;
            }

            if(e & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                OnDisconnect(fd);
                continue;
            }

            if(e & EPOLLIN)
            {
                while(true)
                {
                    char buf[4096 + 1];
                    ssize_t r = recv(fd, buf, 4096, 0);

                    if( r == 0)
                    {
                        OnDisconnect(fd);
                        break;
                    }

                    if(r < 0)
                    {
                        if(errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        OnDisconnect(fd);
                        break;
                    }

                    buf[r] = '\0';
                    OnReceive(fd, buf, static_cast<size_t>(r));
                }
            }
        }
    }

}

void ChannelServer::OnAccept()
{
    while(true)
    {
        sockaddr_in caddr{};
        socklen_t clen = sizeof(caddr);

        int cfd = accept(m_listen_fd, reinterpret_cast<sockaddr*>(&caddr), &clen);

        if(cfd < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            break;
        }

        if(SerNonblocking(cfd) < 0)
        {
            close(cfd);
            continue;
        }

        epoll_event cev{};
        cev.events = EPOLLIN || EPOLLRDHUP;
        cev.data.fd = cfd;

        if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, cfd, &cev) < 0)
        {
            close(cfd);
            continue;
        }

        ChannelSession* session = new ChannelSession(cfd);

        m_sessions[cfd] = session;

        {
            char ip[64]{};
            inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));
            // 로그로 접속한 IP 정보 출력
        }


    }
}

void ChannelServer::OnReceive(int fd, char* buf, size_t len)
{
    auto it = m_sessions.find(fd);
    if(it == m_sessions.end())
    {
        return;
    }

     ChannelSession* session = it->second;
     session->OnBytes((const uint8_t*)buf, len);
}

void ChannelServer::OnDisconnect(int fd)
{
    epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr);

    auto it = m_sessions.find(fd);
    if(it != m_sessions.end())
    {
        delete it->second;
        m_sessions.erase(it);
    }

    close(fd);

    std::cout << "Disconnected FD [" << fd <<"]" << std::endl;
}

void ChannelServer::BroadCast()
{

}

void ChannelServer::SendHeartbeatToWorld()
{

}