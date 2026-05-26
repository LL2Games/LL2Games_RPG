#include "InventoryPacketHandler.h"
#include "ChannelSession.h"
#include "PacketParser.h"
#include "InventroyManager.h"
#include "InventoryPacketSender.h"
#include "Player.h"


void InventoryPacketHandler::Execute(PacketContext * ctx)
{
    ChannelSession *session = nullptr;
    Player* player = nullptr;
    InventoryManager* inventoryManager = nullptr;
    int rc = EXIT_SUCCESS;
    
    size_t offset = 0;
    std::string errMsg;
    std::string str_inventoryType;
    std::string str_fromSlotPos;
    std::string str_toSlotPos;
    bool result = false;

    std::vector<InventorySlotUpdate> inventorySlotUpdate;

    MoveItem moveData{};

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

   inventoryManager = player->GetInventoryManager();

   if(inventoryManager == nullptr)
   {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] inventoryManager is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]inventoryManager is nullptr";
        goto err;
   }
   // inventoryType 
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

    //fromSlotPos 
    if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_fromSlotPos,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    //toSlotPos 
    if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_toSlotPos,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!utility::StringToInt(str_inventoryType, moveData.inventorytype))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] playerid String To Int Fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!utility::StringToInt(str_fromSlotPos, moveData.fromSlotPos))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] playerid String To Int Fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     if(!utility::StringToInt(str_toSlotPos, moveData.toSlotPos))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] playerid String To Int Fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
   
   result = inventoryManager->MoveItemSlots(moveData, inventorySlotUpdate, errMsg);
   // InventoryManager에서 인벤토리 슬롯 움직이기 함수 실행
   if(!result)
   {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] MoveItemSlots Err [%s]", __FILE__, __FUNCTION__, __LINE__, errMsg);
        goto err;
   }

   K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] InventoryPacketHandler ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ", __FILE__, __FUNCTION__, __LINE__);
err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_INVENTORY_ITEM_MOVE, errMsg);
    } else {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] PKT_INVENTORY_ITEM_MOVE END", __FILE__, __FUNCTION__, __LINE__);
        InventoryPacketSender::SendInventoryMoveItem(session,result, moveData.inventorytype, inventorySlotUpdate, errMsg);
    }
}