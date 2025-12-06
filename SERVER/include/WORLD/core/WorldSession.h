#pragma once
#include <string>

class WorldSession
{
public:
    //Packet.h 형식으로 바꿀예정
    int OnPacket(const std::string& packet);
    int Send(int type, std::string& data);
    int Close(); 
private:
    int m_fd;
    std::string m_account_id;
    std::string m_selected_char_id;
    std::string m_state; //SessionState 구조체로 변경 예정
};