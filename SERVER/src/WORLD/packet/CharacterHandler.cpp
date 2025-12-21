#include "packet/CharacterHandler.h"
#include "CharacterService.h"
#include "WorldSession.h"
#include "K_slog.h"


void CharacterHandler::Execute(PacketContext* ctx)
{
    WorldSession *session = nullptr;
    CharacterService *char_service = nullptr;
    int rc = EXIT_SUCCESS;
    std::string errMsg;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }
    char_service = ctx->char_service;
    if (char_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] char_service is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]char_service is nullptr";
        goto err;
    }
    

err:
    if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_SELECT_CHARACTER, errMsg);
    }
    else
    {
        std::vector<std::string> char_list = char_service->GetCharacterList(session->GetID());
        session->Send(PKT_SELECT_CHARACTER, char_list);
    }
}
