#include "CHANNEL/core/ChannelServer.h"
#include "CHANNEL/core/common.h"

ChannelServer::ChannelServer() : m_channel_id(0), m_listen_fd(0)
{
    
}

ChannelServer::~ChannelServer()
{

}


int ChannelServer::Init(const int port)
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(m_listen_fd < 0)
    {
        std::cout << "Server Error" << std::endl;
        return false;
    }

    int opt = 1;
    setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if(bind(m_listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cout << "bind failed" << std::endl;
        return false;
    }

    // backlog : 클라이언트가 connect를 완료했지만 서버가 아직 accept하지 못한 연결을 커널이 대기 큐에 보관할 수 있는 최대 개수
    // 대기 큐가 꽉 차면 이후 connect는 지연되거나 실패할 수 있다. 따라서 대부분 서버의 최대 개수로 설정한다.
    // 실제 적용 값은 커널 상한(net.core.somaxconn 등)으로 인해 줄어들 수 있다.
    // 동시 접속자 수 제한은 backlog가 아니라 세선 수에서 별도로 제한한다.
    if(listen(m_listen_fd, 1024) < 0)
    {
        std::cout << "listen failed" << std::endl;
        return false;
    }

    //K_slog_trace(K_SLOG_TRACE, "[%s] Listening on %d", DAEMON_NAME, port);

    return 1;


}

void ChannelServer::Run()
{
    fd_set reads;

    while(true)
    {
        FD_ZERO(&reads);
        FD_SET(m_listen_fd, &reads);

        int fd_max = m_listen_fd;
        //for()
        //m_sessions
    }
}

void ChannelServer::GameLoop()
{


}

void ChannelServer::OnAccept()
{

}

void ChannelServer::OnReceive(int fd, char* buf)
{
    (void)fd;
    (void)buf;
   
}

void ChannelServer::OnDisconnect(int fd)
{
    (void)fd;
}

void ChannelServer::BroadCast()
{

}

void ChannelServer::SendHeartbeatToWorld()
{

}