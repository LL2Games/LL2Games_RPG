#include "packet/CharacterHandler.h"
#include "K_slog.h"


void CharacterHandler::Execute(PacketContext* ctx)
{
    //character select logic
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] ctx[%p]", __FUNCTION__, __LINE__, ctx);
    return ;
}
