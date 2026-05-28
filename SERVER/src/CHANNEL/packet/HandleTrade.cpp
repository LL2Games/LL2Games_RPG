#include "PlayerHandler.h"
#include "ChannelSession.h"
#include "Player.h"
#include "PacketParser.h"
#include "PlayerManager.h"
#include "TradeService.h"
#include "PlayerService.h"

void PlayerHandler::HandleTradeRequest(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    ChannelSession *session = nullptr;
    Player *player = nullptr;
    Player *target_player = nullptr;
    size_t offset = 0;
    std::string target_player_name;
    PlayerManager* player_manager = nullptr;
    TradeService* trade_service = nullptr;
    std::string errMsg;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->channel_session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    player_manager = ctx->player_manager;
    if (player_manager == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player_manager is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player_manager is nullptr";
        goto err;
    }

    trade_service = ctx->trade_service;
    if (trade_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] trade_service is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]trade_service is nullptr";
        goto err;
    }

    //교환신청 상대방 Player Name 추출
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            target_player_name,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }

    //교환신청 상대방 Player 객체 추출
    target_player = player_manager->GetPlayer(target_player_name);
    if (target_player == nullptr)
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] target_player is nullptr\n", __FUNCTION__, __LINE__);
        errMsg = "[" + std::to_string(rc) + "]target_player is nullptr";
        goto err;
    }

    rc = trade_service->Request(player, target_player, errMsg);

err:

if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_TRADE_REQUEST, errMsg);
    }
    else
    {
        session->SendOk(PKT_TRADE_REQUEST);
    }

}

void PlayerHandler::HandleTradeAccept(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    ChannelSession *session = nullptr;
    Player *player = nullptr;
    Player *request_player = nullptr;
    size_t offset = 0;
    std::string request_player_id;
    PlayerManager* player_manager = nullptr;
    TradeService* trade_service = nullptr;
    std::string errMsg;
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);
    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->channel_session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    player_manager = ctx->player_manager;
    if (player_manager == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player_manager is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player_manager is nullptr";
        goto err;
    }

    trade_service = ctx->trade_service;
    if (trade_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] trade_service is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]trade_service is nullptr";
        goto err;
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

    //교환신청 상대방 Player Name 추출
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            request_player_id,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST request_player_id[%s]", __FUNCTION__, __LINE__, request_player_id.c_str());

    //교환신청 요청 Player 객체 추출
    request_player = player_manager->GetPlayer(atoi(request_player_id.c_str()));
    if (request_player == nullptr)
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] request_player is nullptr\n", __FUNCTION__, __LINE__);
        errMsg = "[" + std::to_string(rc) + "]request_player is nullptr";
        goto err;
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

    rc = trade_service->Start(request_player, player, errMsg);
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

err:

if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_TRADE_ACCEPT, errMsg);
    }
    else
    {
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST SUCCESS", __FUNCTION__, __LINE__);

        //성공시에는 교환 실행 패킷이 trade_service->Start에서 갈 예정
        //session->SendOk(PKT_TRADE_ACCEPT);
    }

}


void PlayerHandler::HandleTradeReady(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    ChannelSession *session = nullptr;
    Player *player = nullptr;
    std::string target_player_id;
    Player *target_player = nullptr;
    PlayerManager* player_manager = nullptr;
    size_t offset = 0;
    std::vector<TradeItem> trade_items;
    TradeService* trade_service = nullptr;
    std::string errMsg;
    PlayerService* player_service = nullptr;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->channel_session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    player_manager = ctx->player_manager;
    if (player_manager == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player_manager is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player_manager is nullptr";
        goto err;
    }

    player_service = ctx->player_service;
    if (player_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player_service is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player_service is nullptr";
        goto err;
    }

    trade_service = ctx->trade_service;
    if (trade_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] trade_service is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]trade_service is nullptr";
        goto err;
    }

    //교환신청 상대방 Player id추출
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            target_player_id,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }

    //교환신청 요청 Player 객체 추출
    target_player = player_manager->GetPlayer(atoi(target_player_id.c_str()));
    if (target_player == nullptr)
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] target_player is nullptr\n", __FUNCTION__, __LINE__);
        errMsg = "[" + std::to_string(rc) + "]target_player is nullptr";
        goto err;
    }

    //Item추출
    //반복하여 ItemId, amount 추출
#if 0 //gunoo22 260528 AddItem에서 아이템 업로드로 변경
    while (1)
    {
        std::string item_id;
        std::string item_amount;

        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

        //item_id
        if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            item_id,
            errMsg))
        {
            //더이상 없으면 중단
                    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);
            break;
        }

                K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

        //item_amount
        if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            item_amount,
            errMsg))
        {
            //error
                    K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

            break;
        }

        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

        TradeItem trade_item;
        trade_item.id = item_id;
        trade_item.type = item_id.substr(0, 1); //id의 첫글자로 타입 구분 ex) 2000000 -> type 2 
        trade_item.amount = atoi(item_amount.c_str());

        trade_items.push_back(trade_item);
    }
#endif //gunoo22 260528 AddItem에서 아이템 업로드로 변경

        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

    rc = trade_service->Ready(player, trade_items, errMsg);
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]gunoo22_TEST", __FUNCTION__, __LINE__);

err:
    if (rc == EXIT_SUCCESS)
    {
        //DB업데이트에 따라 각 Player의 Inventory 업데이트
        player_service->LoadInventory(player);
        player_service->LoadInventory(target_player);
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d]player inventories loaded", __FUNCTION__, __LINE__);
        //TODO
        //UpdateInventory로 변경

        //상대 player에게 교환 성사 패킷 전송
        target_player->GetSession()->SendOk(PKT_TRADE_CONFIRM);
        session->SendOk(PKT_TRADE_CONFIRM);
        
    }
    else if (rc == 2) //상대 교환준비 대기
    {
        //상대 player에게 교환 준비 상태 패킷 전송
        target_player->GetSession()->Send(PKT_TRADE_READY, {std::to_string(player->GetId())});
        session->Send(PKT_TRADE_READY, {"wait"});
    }
    else 
    {
        session->SendNok(PKT_TRADE_READY, errMsg);
    }
}

void PlayerHandler::HandleTradeCancel(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    ChannelSession *session = nullptr;
    Player *player = nullptr;
    TradeService* trade_service = nullptr;
    std::string target_player_id;
    Player *target_player = nullptr;
    PlayerManager* player_manager = nullptr;
    size_t offset = 0;
    std::string errMsg;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->channel_session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    player_manager = ctx->player_manager;
    if (player_manager == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player_manager is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player_manager is nullptr";
        goto err;
    }

    trade_service = ctx->trade_service;
    if (trade_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] trade_service is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]trade_service is nullptr";
        goto err;
    }

    //교환신청 상대방 Player id추출
    if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            target_player_id,
            errMsg))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
        goto err;
    }

    //교환신청 상대방 Player 객체 추출
    target_player = player_manager->GetPlayer(atoi(target_player_id.c_str()));
    if (target_player == nullptr)
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] target_player is nullptr\n", __FUNCTION__, __LINE__);
        errMsg = "[" + std::to_string(rc) + "]target_player is nullptr";
        goto err;
    }


    rc = trade_service->Cancel(player, errMsg);

err:

    if (rc != EXIT_SUCCESS)
    {
        session->SendNok(PKT_TRADE_CANCEL, errMsg);
    }
    else
    {
        //상대 player에게 교환 성사 패킷 전송
        target_player->GetSession()->SendOk(PKT_TRADE_CANCEL);
        session->SendOk(PKT_TRADE_CANCEL);
    }
}

void PlayerHandler::HandleTradeAddItem(PacketContext* ctx)
{
    int rc = EXIT_SUCCESS;
    ChannelSession *session = nullptr;
    Player *player = nullptr;
    Player *target_player = nullptr;
    PlayerManager* player_manager = nullptr;
    size_t offset = 0;
    TradeService* trade_service = nullptr;
    TradeItem trade_item;
    std::string errMsg;

    if (ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ctx is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }
    session = ctx->channel_session;
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]session is nullptr";
        goto err;
    }

    player = session->GetPlayer();
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player is nullptr";
        goto err;
    }

    player_manager = ctx->player_manager;
    if (player_manager == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player_manager is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]player_manager is nullptr";
        goto err;
    }

    trade_service = ctx->trade_service;
    if (trade_service == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] trade_service is nullptr\n", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]trade_service is nullptr";
        goto err;
    }

    //교환신청 요청 Player 객체 추출
    target_player = trade_service->GetTargetPlayer(player);
    if (target_player == nullptr)
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] target_player is nullptr\n", __FUNCTION__, __LINE__);
        errMsg = "[" + std::to_string(rc) + "]target_player is nullptr";
        goto err;
    }

    //Item추출
    {
        std::string item_id, item_amount, item_slot_index;

        //item_id
        if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            item_id,
            errMsg))
        {
            rc = EXIT_FAILURE;
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
            goto err;
        }

        //item_amount
        if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            item_amount,
            errMsg))
        {
            rc = EXIT_FAILURE;
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
            goto err;
        }

        //item_slot_index
        if (!PacketParser::ParseLengthPrefixedString(
            ctx->payload,
            ctx->payload_len,
            offset,
            item_slot_index,
            errMsg))
        {
            rc = EXIT_FAILURE;
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] ParseLengthPrefixedString fail", __FUNCTION__, __LINE__);
            goto err;
        }

        trade_item.id = item_id;
        trade_item.type = item_id.substr(0, 1); //id의 첫글자로 타입 구분 ex) 2000000 -> type 2 
        trade_item.amount = std::stoi(item_amount);
        trade_item.slot_index = std::stoi(item_slot_index);

        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] player[%s] uploads item", __FUNCTION__, __LINE__, player->GetName().c_str());
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] trade_item.id: %s", __FUNCTION__, __LINE__, trade_item.id.c_str());
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] trade_item.type: %s", __FUNCTION__, __LINE__, trade_item.type.c_str());
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] trade_item.amount: %d", __FUNCTION__, __LINE__, trade_item.amount);
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] trade_item.slot_index: %d", __FUNCTION__, __LINE__, trade_item.slot_index);

        rc = trade_service->UploadItem(player, trade_item, errMsg);
    }

err:
    if (rc == EXIT_SUCCESS)
    {
        //상대 player에게 아이템 업로드 패킷 전송
        std::vector<std::string> payload = {
            trade_item.id,
            std::to_string(trade_item.amount),
            std::to_string(trade_item.slot_index)
        };

        target_player->GetSession()->Send(PKT_TRADE_ADD_ITEM, payload);
        session->SendOk(PKT_TRADE_ADD_ITEM);
    }
    else 
    {
        session->SendNok(PKT_TRADE_ADD_ITEM, errMsg);
    }
}
