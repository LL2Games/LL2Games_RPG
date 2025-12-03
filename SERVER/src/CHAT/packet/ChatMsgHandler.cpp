#include "CHAT/packet/ChatMsgHandler.h"
#include "Packet.h"
#include <functional>
#include <string>
#include "K_slog.h"
#include "db/MySQLManager.h"

void ChatMsgHandler::Execute(Client *client, const char *payload, const int len, const std::vector<Client *> &clients, std::function<void(const std::string&, const std::string &, const int)>(broadcast))
{
    std::string msg(payload, len);
    K_slog_trace(K_SLOG_TRACE, "[ChatMsg: %d] %s", client->GetFD(), msg.c_str());
    for (int i = 0; i < len; i++)
    {
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] payload[%d]: %x", __FUNCTION__, __LINE__, i, (uint8_t)payload[i]);
    }

    broadcast(client->GetNick(), msg, client->GetFD());

    // temp
    for (auto c : clients)
    {
        c->GetFD();
    }
}