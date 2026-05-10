#include "ChannelServer.h"
#include "common.h"
#include "thread"


#define THREAD_POOL_COUNT 4


ChannelServer::ChannelServer() : m_channel_id(0), m_listen_fd(0), m_epfd(0), m_running(false), m_map_manager(this), m_map_service(m_player_mamager, m_map_manager), m_pool(THREAD_POOL_COUNT)
{
    m_item_manager = ItemManager::GetInstance();
    m_monster_manager = MonsterManager::GetInstance();
    m_skill_manager = SkillManager::GetInstance();
    m_drop_manager = DropManager::GetInstance();
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

    // 서버 구동 시 JSON 파일을 읽어온다.
    if(!m_map_manager.Init()) return false;
    if(!m_item_manager->Init()) return false;
    if(!m_monster_manager->Init()) return false;
    if(!m_skill_manager->Init()) return false;
    if(!m_drop_manager->Init()) return false;


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

   K_slog_trace(K_SLOG_TRACE, "Thread Pool Start\n");
   //스레드풀 시작
   m_pool.Start();
   K_slog_trace(K_SLOG_TRACE, "ChatD MessageQueue Start\n");
   //chatD 메시지큐 리시버 스레드 시작
   m_cmd_receiver.Start();

   
   //맵매니저 스레드 시작
   m_map_manager.Start();

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
    // 서버 시작 시점에는 아직 클라이언트 소켓이 존재하지 않기 때문에,
    // 먼저 listen 소켓을 epoll의 관심 목록에 등록한다.
    // 이렇게 하면 클라이언트의 접속 요청이 들어왔을 때 epoll_wait()를 통해
    // accept 가능한 상태가 되었음을 감지할 수 있다.
    // epoll 감시기 생성 -> 성공하면 0 반환 실패 시 -1 반환
    m_epfd = epoll_create1(EPOLL_CLOEXEC);
    if(m_epfd < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] m_epfd epoll_create Error %d\n", "ChannelServer", m_epfd);
        return false;
    }

    epoll_event ev{};
    ev.events = EPOLLIN; //읽기 가능한 이벤트를 감시하겠다 라고 설정 / 리슨 소켓에서는 read 가능 상태임을 뜻한다.
    ev.data.fd = m_listen_fd; // 어떤 fd에서 이벤트가 발생했는지 식별하기 위해서 설정

    // m_epfd의 관심 목록에 m_listem_fd를 추가하는 함수 / 즉 이 epoll 인스턴스가 이제부터 리슨 소켓을 감시해라 그리고 읽기 이벤트가 오면 알려줘라 라는 뜻이다.
    if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listen_fd, &ev) < 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s] m_epfd epoll 속성 설정 실패 %d\n", "ChannelServer", m_epfd);
        close(m_epfd);
        m_epfd = -1;
        return false;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s] InitEpoll Success \n", "ChannelServer");
    // 이벤트를 받아 담을 공간을 미리 설정
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
        // m_epfd에 등록된 관심 목록에서 이벤트가 발생한 것들을 기다렸다가 m_events 배열에 채워 넣고 발생한 이벤트 개수를 n에 저장/ -1은 무한 대기의 의미 이벤트가 발생할 때 까지 계속 블로킹 
        int n = epoll_wait(m_epfd, m_events.data(), static_cast<int>(m_events.size()), -1);

        if( n < 0)
        {
            // EINTR : 보통 시그널 때문에 잠깐 인터럽트된 경우
            if(errno == EINTR) continue;
            break;
        }

        for(int i = 0; i < n; ++i)
        {
            int fd = m_events[i].data.fd;
            uint32_t e = m_events[i].events;

            // 새로운 클라이언트의 접속이라면 OnAccept 함수 실행
            if(fd == m_listen_fd)
            {
                OnAccept();
                continue;
            }
            
            // EPOLLERR : 소켓 에러 / EPOLLHUP : hang up, 연결 끊김 or 상대와의 연결이 정상 상태가 아님 / EPOLLRDHUP : 상대방이 read 방향 종료, 보통 원격 종료 감지용
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
            // EAGAIN이랑 EWOULDBLOCK은 논블로킹 소켓에서 거의 같은 의미로 본다.
            // EAGAIN : 지금은 자원이 준비되지 않았으니 나중에 다시 시도하라는 의미
            // EWOULDBLOCK : 지금 이 작업을 수행하면 블로킹되는 상태라는 의미
            // 즉, 논블로킹 모드에서는 기다리지 않고 즉시 반환하면서, 지금 당장은 처리할 수 없음을 알려주는 값
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
        // EPOLLIN : 읽기 가능한 상태
        // EPOLLRDHUP : 상대방이 연결의 읽기 방향을 닫았음
        // 이 두개를 같이 설정하는 이유는 클라이언트가 데이터를 보냈거나 연결 종료를 했을 경우 두 경우을 알기 위해서 두 개로 설정
        cev.events = EPOLLIN | EPOLLRDHUP;
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

    //K_slog_trace(K_SLOG_DEBUG, "fd %d\n", fd);
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