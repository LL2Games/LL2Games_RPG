#pragma once
#include <string>
#include <vector>
#include "Packet.h"
struct SessionState
{

};

class WorldSession
{
public:
    WorldSession(const int fd);
    ~WorldSession();

    //Packet.h 형식으로 바꿀예정
    int GetFD () const { return m_fd; }
    std::string GetID () const { return m_account_id; }
    int SetAccountid(const std::string &id) {m_account_id = id; return 0;}
    
    int OnPacket(const std::string& packet);
    int Send(int type, const std::vector<std::string>& payload);
    int Close(); 
    
    std::vector<char> m_recvBuffer;
private:
    int m_fd;
    std::string m_account_id;
    std::string m_selected_char_id;
    SessionState m_state;
};