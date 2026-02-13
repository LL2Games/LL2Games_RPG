#include "common.h"
#include "PacketParser.h"
#include "ChannelSession.h"
#include "PlayerManager.h"
#include "MapInstance.h"
#include "ItemService.h"
#include "PacketDTO.h"


void UseItemPacket(PacketContext * ctx)
{
    ChannelSession *session = nullptr;
    ItemService* item_service = nullptr;
    size_t offset = 0;
    UseItemResult result;
    int rc = EXIT_SUCCESS;

    std::string item_id;
    std::string use_count;
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

    // item_id를 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        item_id,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     // use_count 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        use_count,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    // ITEM 사용 가능 여부 확인 후 사용
    //Player* player, int itemId, int useCount
    rc = item_service->HandleUseItem(session->GetPlayer(), stoi(item_id), stoi(use_count), result);
    if(rc == EXIT_FAILURE)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] HandleUseItem Failed", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    useItem_Info.push_back(std::to_string(result.item_id));
    useItem_Info.push_back(std::to_string(result.used_count));
    useItem_Info.push_back(std::to_string(result.remain_count));
    useItem_Info.push_back(std::to_string(result.hp));
    useItem_Info.push_back(std::to_string(result.mp));
    
    session->Send(PKT_PLAYER_USE_ITEM, useItem_Info);

    
err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_ENTER_MAP, errMsg);
    } else {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] UseItemPacket END", __FILE__, __FUNCTION__, __LINE__);
        session->SendOk(PKT_ENTER_MAP);
    }


}