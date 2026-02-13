#include "common.h"
#include "PacketParser.h"
#include "ChannelSession.h"


 // 플레이어가 공격 했을 때 피격 당하는 몬스터

// 플레이어가 공격을 했을 때 클라이언트에서 피격 여부를 판정하고 누가 피격당했는지 어떤 스킬 및 공격을 수행했는지에 대해서 정보를 전달하는 것이 아니라
// 플레이어가 공격을 했다라는 정보만을 가지고 서버에서 피격 판정 여부와 공격 가능 여부를 판단해서 같은 맵의 플레이어들한테 정보 전달
void AttackPacket(PacketContext * ctx)
{   
    ChannelSession *session = nullptr;
    size_t offset = 0;
    int rc = EXIT_SUCCESS;

    std::string skill_id;
    std::string attack_dir;
    std::string errMsg;
     
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

    // skill_id를 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        skill_id,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

     // 공격 방향을 추출
     if(!PacketParser::ParseLengthPrefixedString(
        ctx->payload,
        ctx->payload_len,
        offset,
        attack_dir,
        errMsg
    ))
    {
        rc = EXIT_FAILURE;
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] ParseLengthPrefixedString fail", __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }

    

    



err:
    if (rc != EXIT_SUCCESS) {
        session->SendNok(PKT_ENTER_MAP, errMsg);
    } else {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] MAP HANDLER END", __FILE__, __FUNCTION__, __LINE__);
        session->SendOk(PKT_ENTER_MAP);
    }

}



int CheckHit()
{
    return 0;
}

