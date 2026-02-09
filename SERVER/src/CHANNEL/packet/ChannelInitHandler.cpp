#include "common.h"
#include "ChannelInitHandler.h"
#include "ChannelSession.h"
#include "PlayerService.h"
#include "Player.h"
#include "PlayerManager.h"
#include "PacketParser.h"
#include "K_slog.h"
#include <sstream>



void ChannelInitHandler::Execute(PacketContext* ctx)
{
    //std::string payload(ctx->payload, ctx->payload_len);
    //auto tokens = ParsePayload(payload);


    // 패킷 타입에 따른 처리 (임시로 주석 처리)
    /*
    switch(패킷타입) {
        case PKT_CHANNEL_AUTH:
            HandleChannelAuth(ctx, tokens);
            break;
        default:
            break;
    }
    */
    
    // 임시로 인증 처리
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] LJH TEST", __FUNCTION__ , __LINE__);   
    HandleChannelAuth(ctx);
}

void ChannelInitHandler::HandleChannelAuth(PacketContext *ctx)
{
    ChannelSession* session;
    std::unique_ptr<Player> player;
    int rc = EXIT_SUCCESS;
    std::string errMsg;
    size_t offset = 0;
    int characterId =0;
    std::string ch_id;
    
    if(ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ctx is nullptr\n", __FILE__, __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        errMsg = "[" + std::to_string(rc) + "]ctx is nullptr";
        goto err;
    }

     // 받은 정보에서 playerID 추출 
    if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        ch_id,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
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


    characterId = stoi(ch_id);

    player = PlayerService::LoadPlayer(characterId);

    if(!player) {
        errMsg = "[" + std::to_string(rc) + "]HandleChannelAuth: 플레이어 로드 실패 [" + std::to_string(characterId) + "]";
        K_slog_trace(K_SLOG_ERROR, "[%d][%s]%s", __LINE__, __FUNCTION__, errMsg.c_str());
        rc = EXIT_FAILURE;
        goto err;
    }

    K_slog_trace(K_SLOG_TRACE, "HandleChannelAuth: 플레이어 로드 성공 [%d]", characterId);

    // 세션에 플레이어 연결
    ctx->channel_session->SetPlayer(player.get());
    player->SetSession(ctx->channel_session);

    K_slog_trace(K_SLOG_DEBUG, " [%s][%d] player_manager [%p]", __FUNCTION__ , __LINE__, ctx->player_manager);  
    // PlayerManager에 등록 (안전 체크)
    if (ctx->player_manager) {
        bool success = ctx->player_manager->AddPlayer(std::move(player));
        if (success) {
            K_slog_trace(K_SLOG_TRACE, "HandleChannelAuth: PlayerManager 등록 성공");
        } else {
            K_slog_trace(K_SLOG_ERROR, "HandleChannelAuth: PlayerManager 등록 실패 (중복?)");
        }
    } else {
        K_slog_trace(K_SLOG_ERROR, "HandleChannelAuth: PlayerManager가 null입니다");
    }
    // 성공 응답
    K_slog_trace(K_SLOG_TRACE, "HandleChannelAuth: 인증 완료");

err:
    if (rc != EXIT_SUCCESS) {
        ctx->channel_session->SendNok(PKT_CHANNEL_AUTH, errMsg);
    } else {
        ctx->channel_session->SendOk(PKT_CHANNEL_AUTH);
    }
}