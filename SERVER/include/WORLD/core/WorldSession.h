#pragma once
#include <string>
#include <vector>
#include "Packet.h"
#include "K_slog.h"
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
    std::string GetID () const { 
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]account_id[%s]", __FUNCTION__, __LINE__, m_account_id.c_str());
        return m_account_id; }
    int SetAccountid(const std::string &id) {
        m_account_id = id;
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]account_id[%s]", __FUNCTION__, __LINE__, m_account_id.c_str());
        return 0;
    }
    
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