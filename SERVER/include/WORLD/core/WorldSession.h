#pragma once
#include <string>
#include <vector>
struct SessionState
{

};

class WorldSession
{
public:
    WorldSession(const int fd) : m_fd(fd) {}
    ~WorldSession() {}

    //Packet.h 형식으로 바꿀예정
    int GetFD () const { return m_fd; }
    int OnPacket(const std::string& packet);
    int Send(int type, std::string& data);
    int Close(); 
    
    std::vector<char> m_recvBuffer;
private:
    int m_fd;
    std::string m_account_id;
    std::string m_selected_char_id;
    SessionState m_state; //SessionState 구조체로 변경 예정
};