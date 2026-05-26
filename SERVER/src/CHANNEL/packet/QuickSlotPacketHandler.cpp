#include "QuickSlotPacketHandler.h"
#include "ChannelSession.h"
#include "PacketParser.h"
#include "Slot_Info.h"
#include "utility.h"
#include "QuickSlotManager.h"
#include "Player.h"
#include "QuickSlotPacketSender.h"

void QuickSlotPacketHandler::Execute(PacketContext * ctx)
{
    HandleSetQuickSlot(ctx);
}

void QuickSlotPacketHandler::HandleSetQuickSlot(PacketContext* ctx)
{
    ChannelSession *session = nullptr;
    Player* player = nullptr;
    QuickSlotManager* quickSlotManager = nullptr;
    size_t offset = 0;

   
    int type =0;
    int rc = EXIT_SUCCESS;

    std::string errMsg;
    std::vector<QuickSlotData> result;
    QuickSlotData quickSlotData{};
    
     
    if(ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ctx is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }

    session = ctx->channel_session;
    if(session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] session is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if(player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] player is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    quickSlotManager = player->GetQuickSlotManager();
    if(quickSlotManager == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] quickSlotManager is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]quickSlotManager is nullptr";
        goto err;
    }

    // 퀵슬롯 인덱스 추출
    if(!PacketParser::ParseNextIntField(ctx->payload,ctx->payload_len,offset,quickSlotData.slot_index,errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseNextIntField fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!PacketParser::ParseNextIntField(ctx->payload,ctx->payload_len,offset,type,errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseNextIntField fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
    
    if(!PacketParser::ParseNextIntField(ctx->payload, ctx->payload_len, offset, quickSlotData.ref_id, errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseNextIntField fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!PacketParser::ParseNextIntField(ctx->payload, ctx->payload_len, offset, quickSlotData.inventory_type, errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseNextIntField fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!PacketParser::ParseNextIntField(ctx->payload, ctx->payload_len, offset, quickSlotData.inventory_slotPos, errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseNextIntField fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!PacketParser::ParseNextIntField(ctx->payload, ctx->payload_len, offset, quickSlotData.count, errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseNextIntField fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
  
    quickSlotData.type = QuickSlot::SetSlotType(type);

    K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d] quickSlotData.type [%d] ", __FILE__, __FUNCTION__, __LINE__,type);
    
    result = quickSlotManager->SetSlot(quickSlotData);

    if (result.empty())
    {
        rc = EXIT_FAILURE;
        errMsg = "SetSlot failed";
        goto err;
    }
err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_QUICKSLOT_SET, errMsg);
    } else {
        QuickSlotPacketSender::SendQuickSlotSet(player, result);
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] QuickSlotSet END", __FILE__, __FUNCTION__, __LINE__);
    
    }

}