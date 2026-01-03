#include "Packet.h"
#include "Client.h"
#include "ChatInitHandler.h"
#include <functional>
#include <string>
#include "K_slog.h"
#include "db/MySQLManager.h"
#include "PacketParser.h"

void ChatInitHandler::Execute(PacketContext *ctx)
{
    int rc = EXIT_SUCCESS;
    std::string errMsg;
    size_t offset = 0;
    Client *client = nullptr;
    std::string id;
    std::string nick;
    MySQLManager &db = MySQLManager::Instance();

    client = ctx->client;
    if (client == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] client is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]client is nullptr";
        goto err;
    }

    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            id,
            errMsg))
    {
        rc = EXIT_FAILURE;
        goto err;
    }

    nick = db.GetNick(id);
    K_slog_trace(K_SLOG_TRACE, "[%s][%d][client fd=%d] id=%s, nick=%s", __FILE__, __LINE__, client->GetFD(), id.c_str(), nick.c_str());
err:
    if (rc != EXIT_SUCCESS)
    {
        client->SendNok(PKT_CHAT_INIT, errMsg);
    }
    else
    {
        client->SetId(id);
        client->SetNick(nick);
        std::vector<std::string> client_info;
        client_info.push_back("id=" + id);
        client_info.push_back("nick=" + nick);
        client->SendOk(PKT_CHAT_INIT, client_info);
    }
}
