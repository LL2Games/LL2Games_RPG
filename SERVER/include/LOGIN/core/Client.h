#pragma once
#include <string>
#include <vector>

// Reactor

class Client{
public:
    Client(int fd) : m_fd(fd) {}

    int GetFD() const { return m_fd; }

    // 버퍼
    std::vector<char> m_recvBuffer;

    std::string m_id;
    std::string m_nick;

private:
    int m_fd;
};