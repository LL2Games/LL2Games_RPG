#include "PlayerHandler.h"
#include "common.h"
#include "PacketParser.h"
#include "ChannelSession.h"
#include "PlayerManager.h"
#include "MapInstance.h"
#include "ItemService.h"
#include "PacketDTO.h"
#include "Inventory_Info.h"
#include "utility.h"
#include "ItemPacketSender.h"



bool TransferData(Str_UseItem& str_itemData, UseItem& itemData);


void PlayerHandler:: UseItemPacket(PacketContext * ctx)
{
    ChannelSession *session = nullptr;
    ItemService* item_service = nullptr;
    size_t offset = 0;
    UseItemResult result;

    Str_UseItem str_itemData{};
    UseItem itemData{};
    int rc = EXIT_SUCCESS;

    std::string str_inventoryType;
    std::string str_slotPos;
    std::string str_itemId;
    std::string str_useCount;
    std::string errMsg;

    std::vector<std::string> useItem_Info;
     
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

    item_service = ctx->item_service;
    if(item_service == nullptr)
    {
     K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] item_service is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "] item_service is nullptr";
        goto err;
    }

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
        str_slotPos,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }


    // item_id를 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_itemId,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     // use_count 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        str_useCount,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    str_itemData.str_inventoryType = str_inventoryType;
    str_itemData.str_slotPos = str_slotPos;
    str_itemData.str_itemId = str_itemId;
    str_itemData.str_useCount =str_useCount;

    if(!TransferData(str_itemData, itemData))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] Transfer fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    // ITEM 사용 가능 여부 확인 후 사용
    //Player* player, int itemId, int useCount

    K_slog_trace(K_SLOG_DEBUG, "[%s : %s : %d] itemData UseCount [%d]", __FILE__, __FUNCTION__, __LINE__, itemData.useCount);
    rc = item_service->HandleUseItem(session->GetPlayer(), itemData, result);
    if(rc == EXIT_FAILURE)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s : %d] HandleUseItem Failed", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
    useItem_Info.push_back(std::to_string(result.result));
    useItem_Info.push_back(std::to_string(result.errcode));
    useItem_Info.push_back(std::to_string(result.inventoryType));
    useItem_Info.push_back(std::to_string(result.slotPos));
    useItem_Info.push_back(std::to_string(result.item_id));
    useItem_Info.push_back(std::to_string(result.used_count));
    useItem_Info.push_back(std::to_string(result.remain_count));
    useItem_Info.push_back(std::to_string(result.hp));
    useItem_Info.push_back(std::to_string(result.mp));
    

err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_PLAYER_USE_ITEM, errMsg);
    } else {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] UseItemPacket END", __FILE__, __FUNCTION__, __LINE__);
        session->Send(PKT_PLAYER_USE_ITEM, useItem_Info);
    }


}

void PlayerHandler::PickUpItemPacket(PacketContext *ctx)
{
    ChannelSession *session = nullptr;
    Player* player = nullptr;
    MapInstance* mapInstance = nullptr;
    size_t offset = 0;
    int dropItemId = 0;
    std::string errMsg;
    std::vector<AddItemResult> addItemResults;
    int rc = EXIT_SUCCESS;
    K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] PickUpItemPacket Start \n", __FILE__, __FUNCTION__, __LINE__);
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
        errMsg = "[" + std::to_string(rc) + "] player is nullptr";
        goto err;
    }

    mapInstance = player->GetCurrentMap();
    if(mapInstance == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] mapInstance is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "] mapInstance is nullptr";
        goto err;
    }

    if(!PacketParser::ParseNextIntField(ctx->payload, ctx->payload_len, offset, dropItemId, errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseNextIntField fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    if(!mapInstance->PickupDropItem(player, dropItemId ,addItemResults))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] PickupDropItem fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
    

  err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_PLAYER_PICKUP_ITEM, errMsg);
    } else {
        ItemPacketSender::SendAddItem(player, addItemResults);
        K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] PickUpItemPacket END", __FILE__, __FUNCTION__, __LINE__);
    }
}



bool TransferData(Str_UseItem& str_itemData, UseItem& itemData)
{
    if(!utility::StringToInt(str_itemData.str_inventoryType , itemData.inventoryType))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] str_inventoryType String to Int fail", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if(!utility::StringToInt(str_itemData.str_useCount , itemData.useCount))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] str_inventoryType String to Int fail", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if(!utility::StringToInt(str_itemData.str_itemId , itemData.itemId))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] str_inventoryType String to Int fail", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if(!utility::StringToInt(str_itemData.str_slotPos , itemData.slotPos))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] str_inventoryType String to Int fail", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    K_slog_trace(K_SLOG_DEBUG, "[%s : %s][%d] itemData.inventoryType [%d]", __FILE__, __FUNCTION__, __LINE__, itemData.inventoryType);
    K_slog_trace(K_SLOG_DEBUG, "[%s : %s][%d] itemData.useCount [%d]", __FILE__, __FUNCTION__, __LINE__, itemData.useCount);
    K_slog_trace(K_SLOG_DEBUG, "[%s : %s][%d] itemData.itemId [%d]", __FILE__, __FUNCTION__, __LINE__, itemData.itemId);
    K_slog_trace(K_SLOG_DEBUG, "[%s : %s][%d] itemData.slotPos [%d]", __FILE__, __FUNCTION__, __LINE__, itemData.slotPos);

    return true;

}