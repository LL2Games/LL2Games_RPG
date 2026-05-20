#include "QuickSlotPacketHandler.h"
#include "ChannelSession.h"
#include "PacketParser.h"
#include "Slot_Info.h"
#include "utility.h"
#include "QuickSlotManager.h"
#include "Player.h"

void QuickSlotPacketHander::HandleSetQuickSlot(PacketContext* ctx)
{
    ChannelSession *session = nullptr;
    Player* player = nullptr;
    QuickSlotManager* quickSlotManager = nullptr;
    size_t offset = 0;

    int type =0;
    int rc = EXIT_SUCCESS;

    std::string errMsg;
    std::string str_slotIndex;
    std::string str_type;
    std::string str_refId;
    std::string str_inventoryType;
    std::string str_inventorySlotPos;

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
    if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_slotIndex,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
    //str_type 퀵슬롯 타입을 추출
    if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_type,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    // str_refId를 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_refId,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     // str_inventoryType 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_inventoryType,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_inventorySlotPos,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
   
    utility::StringToInt(str_slotIndex, quickSlotData.slot_index);
    quickSlotData.type = QuickSlot::SetSlotType(type);
    utility::StringToInt(str_refId, quickSlotData.ref_id);
    utility::StringToInt(str_inventoryType, quickSlotData.inventory_type);
    utility::StringToInt(str_inventorySlotPos, quickSlotData.inventory_slotPos);

    if(!quickSlotManager->SetSlot(quickSlotData))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] SetSlot fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_PLAYER_USE_ITEM, errMsg);
    } else {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] QuickSlotSet END", __FILE__, __FUNCTION__, __LINE__);
    
    }

}