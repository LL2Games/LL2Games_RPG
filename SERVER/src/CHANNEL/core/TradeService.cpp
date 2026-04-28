#include "TradeService.h"
#include "TradeRequestTask.h"
#include "K_slog.h"

TradeService::TradeService()
{
    m_mySql = MySqlConnectionPool::GetInstance();
}
TradeService::~TradeService()
{

}

//int TradeService::HandleTradeRequest(Player* requester, const std::string& target_player_name)
int TradeService::Request(Player* requester, Player* target_player, std::string &errMsg)
{
    //1. 예외처리: target_player와 requestr 같은 맵에 있는지
    if (requester == nullptr || target_player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Invalid player or target player", __FUNCTION__, __LINE__);
        errMsg = "Invalid player or target player";
        return -1;
    }

    //2. 예외처리: target_player가 거래 가능한 상태인지 (ex. 전투중, 피격중, 사망 등등)
    if (!target_player->IsAlive())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Target player is not alive", __FUNCTION__, __LINE__);
        errMsg = "Target player is not alive";
        return -1;
    }

    //3. target_player에게 거래 요청 패킷 보내기
    target_player->GetSession()->Send(PKT_TRADE_REQUEST, {std::to_string(requester->GetId())});

    return 0;
}