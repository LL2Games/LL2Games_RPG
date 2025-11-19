#include "CHAT/packet/ChatMsgHandler.h"
#include "CHAT/packet/Packet.h"
#include <functional>
#include <string>
#include "K_slog.h"

void ChatMsgHandler::Execute(Client *client, const char *payload, const int len, const std::vector<Client *> &clients, std::function<void(const std::string &, const int)>(broadcast))
{
    std::string msg(payload, len);
    K_slog_trace(K_SLOG_TRACE, "[ChatMsg: %d] %s", client->GetFD(), msg.c_str());

    broadcast(msg, client->GetFD());

    // temp
    for (auto c : clients)
    {
        c->GetFD();
    }
}