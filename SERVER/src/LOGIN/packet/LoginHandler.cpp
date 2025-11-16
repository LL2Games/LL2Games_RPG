#include "packet/LoginHandler.h"
#include "db/MySQLManager.h"
#include <sys/socket.h>

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