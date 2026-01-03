#include "Packet.h"
#include "LoginHandler.h"
#include "Client.h"
#include "db/MySQLManager.h"
#include <sys/socket.h>
#include "K_slog.h"
#include "PacketParser.h"

void LoginHandler::Execute(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    std::string errMsg;
    size_t offset = 0;
    Client* client = nullptr;
    std::string id;
    std::string pw;

    client = ctx->client;
    if (client == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] client is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]client is nullptr";
        goto err;
    }

    //id
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            id,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }
    //pw
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            pw,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }
    
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] id=%s pw=%s\n", __FUNCTION__, __LINE__, id.c_str(), pw.c_str());

    if (!MySQLManager::Instance().Login(id, pw))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Login fail invalid ID/PW\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]Login fail invalid ID/PW";
        goto err;
    }

err:
    if (rc != EXIT_SUCCESS)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Err=%s", __FUNCTION__, __LINE__, errMsg.c_str());
        client->SendNok(PKT_LOGIN, errMsg);
    }
    else
    {
        std::string nick = MySQLManager::Instance().GetNick(id);
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] Login SUCCESS ID=%s, NICK=%s\n", __FUNCTION__, __LINE__,id.c_str(), nick.c_str());
        std::vector<std::string> login_info;
        login_info.push_back("id=" + id);
        login_info.push_back("nick=" + nick);
        client->SendOk(PKT_LOGIN, login_info);
    }
}
