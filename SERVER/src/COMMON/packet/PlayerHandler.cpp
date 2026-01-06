#include "CHANNEL/core/common.h"
#include "CHANNEL/packet/PlayerHandler.h"
#include "CHANNEL/core/ChannelSession.h"
#include "CHANNEL/db/PlayerService.h"
#include "CHANNEL/core/Player.h"
#include "CHANNEL/core/PlayerManager.h"
#include "K_slog.h"
#include <sstream>



void PlayerHandler::Execute(PacketContext* ctx)
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

void PlayerHandler::HandleChannelAuth(PacketContext *ctx)
{
    const char* data = nullptr;
    size_t len = 0;
    size_t offset = 0;
    uint8_t value_len = 0;

    int characterId =0;

    std::string ch_id;

    data = ctx->payload;
    len = ctx->payload_len;

    K_slog_trace(K_SLOG_TRACE, " [%s][%d] LJH TEST", __FUNCTION__ , __LINE__); 
    if(offset >= len)
    {
         K_slog_trace(K_SLOG_TRACE, " [%s][%d] 데이터 값 없음", __FUNCTION__ , __LINE__);  
    }

    value_len = static_cast<uint8_t>(data[offset]);
    offset +=1;

    if(offset < len && data[offset] == 0x00)
    {
        offset +=1 ;
    }
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] LJH TEST", __FUNCTION__ , __LINE__); 
    if(offset+value_len > len)
    {
        K_slog_trace(K_SLOG_TRACE, "데이터 값 없음");     
    }
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] LJH TEST", __FUNCTION__ , __LINE__);  
    ch_id.assign(data+offset, value_len);
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] LJH TEST", __FUNCTION__ , __LINE__); 
    characterId = stoi(ch_id);

    K_slog_trace(K_SLOG_TRACE, "HandleChannelAuth: characterid 설정");
    //int characterId = std::stoi(tokens[0]);
    //int characterId = std::atoi(tokens[0].c_str());

    K_slog_trace(K_SLOG_TRACE, "HandleChannelAuth: 캐릭터 ID [%d] 인증 시도", characterId);

    PlayerService playerService;
    auto player = playerService.LoadPlayer(characterId);

    if(!player) {
        K_slog_trace(K_SLOG_ERROR, "HandleChannelAuth: 플레이어 로드 실패 [%d]", characterId);
        return;
    }

    K_slog_trace(K_SLOG_TRACE, "HandleChannelAuth: 플레이어 로드 성공 [%d]", characterId);

    // 세션에 플레이어 연결
    ctx->session->SetPlayer(player.get());

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

    ctx->session->SendOk(PKT_CHANNEL_AUTH);
  
}