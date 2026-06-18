#pragma once
#include <vector>
#include <string>

class Client{
public:
    Client(int fd);
    ~Client();

    int SetId(const std::string id) { m_id = id; return 0; }
    const std::string& GetId() const { return m_id; }

    int SetNick(const std::string nick) { m_nick = nick; return 0; }
    const std::string& GetNick() const { return m_nick; }

    int GetFD() const { return m_fd; }

    // 버퍼
    std::vector<char> m_recvBuffer;

    int SendOk(const int type, std::vector<std::string> payload={});
    int SendNok(const int type, const std::string &errMsg);
 
private:
    int m_fd;
    std::string m_id;
    std::string m_nick;
};