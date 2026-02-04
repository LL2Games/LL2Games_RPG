#include "ChannelServer.h"
#include "common.h"

ChannelServer::ChannelServer() : m_channel_id(0), m_listen_fd(0), m_epfd(0), m_running(false), m_map_service(m_player_mamager, m_map_manager)
{
    
}

ChannelServer::~ChannelServer()
{

}

int ChannelServer::SetNonblocking(int fd)
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
    K_slog_trace(K_SLOG_TRACE, "[%s] Channel Server Init %d\n", "ChannelServer", port); 
   if(!InitListenSocket(port))
   {
        K_slog_trace(K_SLOG_ERROR, "[%s] InitListenSocket %d\n", "ChannelServer", port); 
        return false;
   }

   if(!InitEpoll())
   {
        K_slog_trace(K_SLOG_ERROR, "[%s] InitEpoll Error %d\n", "ChannelServer", port); 
        return false;
   }

   return true;


}

bool ChannelServer::InitListenSocket(int port)
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(m_listen_fd < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] m_listen_fd Set Error %d\n", "ChannelServer", port); 
        return false;
    }

    int opt = 1;
    if(setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] m_listen_fd Setsockopt Error %d\n", "ChannelServer", port);
        close(m_listen_fd);
        m_listen_fd =-1;
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if(bind(m_listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))< 0)
    {   
        K_slog_trace(K_SLOG_ERROR, "[%s][%s][%d] m_listen_fd bind Error %d\n", "ChannelServer", __FILE__, __LINE__,  port);
        close(m_listen_fd);
        m_listen_fd = -1;
        return false;
    }

    if(listen(m_listen_fd, 1024) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%s][%d] m_listen_fd listen Error %d\n", "ChannelServer", __FILE__, __LINE__, port);
        close(m_listen_fd);
        m_listen_fd = -1;
        return false;
    }

    if(SetNonblocking(m_listen_fd) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] SerNonblocking Error %d\n", "ChannelServer", port);
        close(m_listen_fd);
        m_listen_fd = -1;
        return false;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s] Listening on Port %d\n", "ChannelServer", port);
    return true;


}

bool ChannelServer::InitEpoll()
{
    m_epfd = epoll_create1(EPOLL_CLOEXEC);
    if(m_epfd < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] m_epfd epoll_create Error %d\n", "ChannelServer", m_epfd);
        return false;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = m_listen_fd;

    if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listen_fd, &ev) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] m_epfd epoll 속성 설정 실패 %d\n", "ChannelServer", m_epfd);
        close(m_epfd);
        m_epfd = -1;
        return false;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s] InitEpoll Success \n", "ChannelServer");
    m_events.resize(64);
    return true;
}

void ChannelServer::Run()
{
    if(m_listen_fd < 0 || m_epfd < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] m_listen_fd or m_epfd is Not initialized %d\n", "ChannelServer", m_epfd);
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
                OnReceive(fd);
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

        if(SetNonblocking(cfd) < 0)
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

        ChannelSession* session = new ChannelSession(cfd, this);

        m_sessions[cfd] = session;

        {
            char ip[64]{};
            inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));
            // 로그로 접속한 IP 정보 출력
        }


    }
}

void ChannelServer::OnReceive(int fd)
{
    char temp[BUFFER_SIZE];
    int tempLen = 0;
    std::string buf;

    do
    {
        memset(temp, 0x00, sizeof(temp));
        tempLen = recv(fd, temp, sizeof(temp), 0);
        if (tempLen <= 0)
        {
            OnDisconnect(fd);
            K_slog_trace(K_SLOG_TRACE, "Client %d disconnected\n", fd);
            close(fd);
            delete m_sessions[fd];
            m_sessions.erase(fd);
            return ;
        }
        buf.append(temp, tempLen);
    } while (tempLen == BUFFER_SIZE);

     K_slog_trace(K_SLOG_DEBUG, "fd %d\n", fd);
    auto it = m_sessions.find(fd);
    if(it == m_sessions.end())
    {
        return;
    }

     ChannelSession* session = it->second;
     if(!session->OnBytes((const uint8_t*)buf.c_str(), buf.size()))
     {
        OnDisconnect(fd);
     }
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