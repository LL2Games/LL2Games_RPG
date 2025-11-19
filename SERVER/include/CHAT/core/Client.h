#pragma once
#include <vector>
#include <string>

class Client {
public:
    Client(const int fd) : m_fd(fd) {}
    int GetFD() const {return m_fd;}
    std::string GetNICK() const {return m_nick;}
    std::vector<char> m_recvBuffer;
    
    private:
    std::string  m_nick;
    int m_fd;
};