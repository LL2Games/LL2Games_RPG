#include "packet/CharacterHandler.h"
#include "CharacterService.h"
#include "WorldSession.h"
#include "K_slog.h"


void CharacterHandler::Execute(PacketContext* ctx)
{
    CharacterService *char_service = nullptr;
    WorldSession *session = nullptr;
    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        return;
    }
    char_service = ctx->char_service;
    if (char_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] char_service is nullptr\n", __FUNCTION__, __LINE__);
        return;
    }
    session = ctx->session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        return;
    }

    std::vector<std::string> char_list = char_service->GetCharacterList(session->GetID());
    session->Send(PKT_SELECT_CHARACTER, char_list);


    return ;
}
