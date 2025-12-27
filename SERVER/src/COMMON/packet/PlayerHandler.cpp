#include "CHANNEL/core/common.h"
#include "CHANNEL/packet/PlayerHandler.h"
#include "CHANNEL/core/ChannelSession.h"
#include "CHANNEL/db/PlayerService.h"
#include "CHANNEL/core/Player.h"
#include "CHANNEL/core/PlayerManager.h"
#include "K_slog.h"
#include <sstream>

std::vector<std::string> PlayerHandler::ParsePayload(const std::string& payload)
{
    std::vector<std::string> tokens;
    std::stringstream ss(payload);
    std::string token;

    while(std::getline(ss, token, '$')) {
        tokens.push_back(token);
    }

    return tokens;
}

void PlayerHandler::Execute(PacketContext* ctx)
{
    std::string payload(ctx->payload, ctx->payload_len);
    auto tokens = ParsePayload(payload);

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
    HandleChannelAuth(ctx, tokens);
}

void PlayerHandler::HandleChannelAuth(PacketContext *ctx, const std::vector<std::string>& tokens)
{
    if(tokens.size() < 1) {
        K_slog_trace(K_SLOG_ERROR, "HandleChannelAuth: 인증 정보가 부족합니다.");
        return;
    }
    
    int characterId = std::stoi(tokens[0]);
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
}