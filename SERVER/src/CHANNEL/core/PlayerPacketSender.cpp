#include "PlayerPacketSender.h"
#include "ChannelSession.h"
#include "CharacterStat.h"
#include "K_slog.h"


void PlayerPacketSender::SendPlayerInfo(Player* player)
{
    auto session = player->GetSession();
    if(!session) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session이 nullptr입니다.", __FUNCTION__, __LINE__);
        return;
    }

    std::vector<std::string> payload;

    payload.push_back(std::to_string(player->GetId()));
    payload.push_back(player->GetName());
    payload.push_back(std::to_string(player->GetJob()));
    payload.push_back(std::to_string(player->GetLevel()));
    payload.push_back(std::to_string(player->GetMapId()));
    payload.push_back(std::to_string(player->GetPos().xPos));
    payload.push_back(std::to_string(player->GetPos().yPos));
    session->Send(PKT_PLAYER_INFO, payload);
}


void PlayerPacketSender::SendPlayerStat(Player* player)
{
    auto session = player->GetSession();

   if(!session) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session이 nullptr입니다.", __FUNCTION__, __LINE__);
        return;
    }

    auto playerStat = player->GetStat();
    auto playerBastStat = playerStat.GetBase();
    std::vector<std::string> payload;

    // 나중에 스탯 정보에 공격력도 들어가야함
    payload.push_back(std::to_string(playerStat.GetMaxHp()));
    payload.push_back(std::to_string(playerStat.GetMaxMp()));
    payload.push_back(std::to_string(playerStat.GetCurHp()));
    payload.push_back(std::to_string(playerStat.GetCurMp()));
    payload.push_back(std::to_string(playerBastStat.str));
    payload.push_back(std::to_string(playerBastStat.dex));
    payload.push_back(std::to_string(playerBastStat.intel));
    payload.push_back(std::to_string(playerBastStat.luck));
    payload.push_back(std::to_string(playerStat.GetRemainAp()));


    session->Send(PKT_PLAYER_STAT, payload);
}