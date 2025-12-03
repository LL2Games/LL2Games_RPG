#include "CHAT/packet/ChatInitHandler.h"
#include "Packet.h"
#include <functional>
#include <string>
#include "K_slog.h"
#include "db/MySQLManager.h"

void ChatInitHandler::Execute(Client *client, const char *payload, const int len, const std::vector<Client *> & = std::vector<Client*>(), std::function<void(const std::string&, const std::string &, const int)> = nullptr)
{
    MySQLManager& db = MySQLManager::Instance();
    std::string id(payload, len);
    client->SetId(id);
    client->SetNick(db.GetNick(id));
    K_slog_trace(K_SLOG_TRACE, "[client fd=%d] id=%s, nick=%s", client->GetFD(), id.c_str(), client->GetNick().c_str());
    for (int i = 0; i < len; i++)
    {
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] payload[%d]: %x", __FUNCTION__, __LINE__, i, (uint8_t)payload[i]);
    }

}
