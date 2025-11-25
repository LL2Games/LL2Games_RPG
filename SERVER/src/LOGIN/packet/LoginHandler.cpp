#include "packet/LoginHandler.h"
#include "db/MySQLManager.h"
#include <sys/socket.h>

#if 1 /*gunoo22 251123 임시*/
void LoginHandler::Execute(Client* client, const char* payload, const int len)
{
    std::string data(payload, len);

    // "id$pw$" 구조 확인
    size_t pos1 = data.find('$');
    if (pos1 == std::string::npos)
    {
        send(client->GetFD(), "NOK$InvalidFormat$", 19, 0);
        return;
    }

    size_t pos2 = data.find('$', pos1 + 1);
    if (pos2 == std::string::npos)
    {
        send(client->GetFD(), "NOK$InvalidFormat$", 19, 0);
        return;
    }

    std::string id = data.substr(0, pos1);
    std::string pw = data.substr(pos1 + 1, pos2 - (pos1 + 1));

    bool ok = MySQLManager::Instance().Login(id, pw);

    if (ok)
    {
        client->m_id = id;
        client->m_nick = MySQLManager::Instance().GetNick(id);
        send(client->GetFD(), "OK$", 3, 0);
    }
    else
    {
        send(client->GetFD(), "NOK$Invalid ID/PW$", 19, 0);
    }
}

#else
void LoginHandler::Execute(Client* client, const char* payload, const int len)
{
    (void)len;
    std::string id = payload;
    std::string pw = payload + id.size() + 1;
    bool ok = MySQLManager::Instance().Login(id, pw);
    
    if (ok)
    {
        client->m_id = id;
        client->m_nick = MySQLManager::Instance().GetNick(id);
        send(client->GetFD(), "OK$", 3, 0);        
    }
    else
    {
        send(client->GetFD(), "NOK$Invalid ID/PW$", 19, 0);
    }
}
#endif