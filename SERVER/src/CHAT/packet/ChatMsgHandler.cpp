#include "ChatMsgHandler.h"
#include "Packet.h"
#include "Client.h"
#include <functional>
#include <string>
#include "K_slog.h"
#include "db/MySQLManager.h"
#include "PacketParser.h"

// void ChatMsgHandler::Execute(Client *client, const char *payload, const int data_len, const std::vector<Client *> &clients, std::function<void(const std::string&, const std::string &, const int)>(broadcast))
void ChatMsgHandler::Execute(PacketContext *ctx)
{
    int rc = EXIT_SUCCESS;
    std::string errMsg;
    size_t offset = 0;
    Client *client = nullptr;
    std::vector<Client *> clients = *ctx->clients;
    std::function<void(const std::string &, const std::string &, const int)> &broadcast = ctx->broadcast;
    std::string msg;

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
            msg,
            errMsg))
    {
        rc = EXIT_FAILURE;
        goto err;
    }

    K_slog_trace(K_SLOG_TRACE, "[ChatMsg: %d] %s", client->GetFD(), msg.c_str());

err:
    if (rc != EXIT_SUCCESS)
    {
        client->SendNok(PKT_CHAT, errMsg);
    }
    else
    {
        broadcast(client->GetNick(), msg, client->GetFD());
        // temp
        for (auto c : clients)
        {
            c->GetFD();
        }
        std::vector<std::string> chatMsg;
        chatMsg.push_back("msg=" + msg);
        client->SendOk(PKT_CHAT, chatMsg);
    }
}