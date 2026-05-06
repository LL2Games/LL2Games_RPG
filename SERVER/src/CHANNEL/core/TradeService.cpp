#include "TradeService.h"
#include "TradeRequestTask.h"
#include "MapInstance.h"
#include "MySqlConnectionPool.h"
//#include "RedisClient.h"
#include "K_slog.h"

std::unordered_map<int, TradeSession *> TradeService::m_sessions;

TradeService::TradeService()
{
    m_mySql = MySqlConnectionPool::GetInstance();
    //m_redis = RedisClient::GetInstance();
}
TradeService::~TradeService()
{
}

// int TradeService::HandleTradeRequest(Player* requester, const std::string& target_player_name)
int TradeService::Request(Player *requester, Player *target_player, std::string &errMsg)
{
    // 1. 예외처리: target_player와 requester 객체가 유효한지
    if (requester == nullptr || target_player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Invalid player or target player", __FUNCTION__, __LINE__);
        errMsg = "Invalid player or target player";
        return -1;
    }

    // 2. 예외처리: target_player가 거래 가능한 상태인지 (ex. 전투중, 피격중, 사망 등등)
    if (!target_player->IsAlive())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Target player is not alive", __FUNCTION__, __LINE__);
        errMsg = "Target player is not alive";
        return -1;
    }

    // 3. target_player에게 거래 요청 패킷 보내기
    target_player->GetSession()->Send(PKT_TRADE_REQUEST, {std::to_string(requester->GetId())});

    return 0;
}

int TradeService::Start(Player *requester, Player *accepter, std::string &errMsg)
{

    // 1. 예외처리: target_player와 requester 객체가 유효한지
    if (requester == nullptr || accepter == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Invalid player or target player", __FUNCTION__, __LINE__);
        errMsg = "Invalid player or target player";
        return -1;
    }

    // 2-1. 예외처리: accepter와 requester가 맵 안에 있는지
    if (!(requester->GetCurrentMap() && accepter->GetCurrentMap()))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] players mapInstance Invalid", __FUNCTION__, __LINE__);
        errMsg = "players mapInstance Invalid";
        return -1;
    }
    // 2-2. 예외처리: accepter와 requester 같은 맵에 있는지
    if (requester->GetCurrentMap()->GetMapId() != accepter->GetCurrentMap()->GetMapId())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Players map different", __FUNCTION__, __LINE__);
        errMsg = "Players map different";
        return -1;
    }

    // 3. 예외처리: accepter와 requester가 거래 가능한 상태인지 (ex. 전투중, 피격중, 사망 등등)
    if (!requester->IsAlive() || !accepter->IsAlive())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is not alive", __FUNCTION__, __LINE__);
        errMsg = "player is not alive";
        return -1;
    }

    // 4. TradeSession 생성
    CreateTradeSession(requester, accepter);

    // 5. player들에게 교환 실행 패킷 보내기
    requester->GetSession()->Send(PKT_TRADE_START, {std::to_string(accepter->GetId())});
    accepter->GetSession()->Send(PKT_TRADE_START, {std::to_string(requester->GetId())});

    return 0;
}

int TradeService::Ready(Player* player, const std::vector<TradeItem>& items, std::string &errMsg)
{
    TradeSession *session = nullptr;
    // 1. 예외처리: player 객체가 유효한지
    if (player == nullptr )
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Invalid player", __FUNCTION__, __LINE__);
        errMsg = "Invalid player";
        return -1;
    }

    session = m_sessions[player->GetId()];
    // 2. 예외처리: player의 TradeSession 유효하지않음
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player trade session Not Found", __FUNCTION__, __LINE__);
        errMsg = "player trade session Not Found";
        return -1;
    }

    // 3. Ready 처리 및 Item 등록
    if (session->a_id == player->GetId())
    {
        session->a_ready = true;
        session->a_items = items;
    }
    else
    {
        session->b_ready = true;
        session->b_items = items;
    }

    // 4. 상대방 Ready 확인 후 교환 수행
    if ((session->a_id == player->GetId() && session->b_ready == true)
    || (session->b_id == player->GetId() && session->a_ready == true))
    {
        return Execute(session);
    }

    return 1; //상대방 교환 준비 대기
}

int TradeService::Execute(TradeSession *session)
{
    MYSQL *conn = m_mySql->GetConnection();
    std::string query;
    int result = 0;

    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr", __FUNCTION__, __LINE__);
        return -1;
    }

    if (!conn)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] MYSQL GetConnection failed", __FUNCTION__, __LINE__);
        return -1;
    }

    mysql_query(conn, "START TRANSACTION");

    //0. 아이템 검증(예외처리)
    //A player DB에서 A items 존재 및 수량 확인
    //B player DB에서 B items 존재 및 수량 확인

    //1. 아이템 교환
    //A player DB에서 A items 임시 제거
    //B player DB에서 B items 임시제거

    //2. 교환 실패(인벤창 예외처리)
    //A player DB에 B items 추가시 인벤창 여유 확인
    //B player DB에 A items 추가시 인벤창 여유 확인

    //3. 교환 성사
    //A player DB에 A items 삭제 및 B items 추가
    //B player DB에 B items 삭제 및 A items 추가

    for (const auto& item : session->a_items)
    {
        //3-1 A인벤에 A의 아이템 삭제
        if (DecreaseItem(conn, std::to_string(session->a_id), item) != 0)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] DecreaseItem failed", __FUNCTION__, __LINE__);
            goto err;
        }

        //3-2 B인벤에 A의 아이템 추가
        if (IncreaseItem(conn, std::to_string(session->b_id), item) != 0)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] IncreaseItem failed", __FUNCTION__, __LINE__);
            goto err;
        }
    }

    for (const auto& item : session->b_items)
    {
        //3-3 B인벤에 B의 아이템 삭제
        if (DecreaseItem(conn, std::to_string(session->b_id), item) != 0)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] DecreaseItem failed", __FUNCTION__, __LINE__);
            goto err;
        } 
    
        //3-4 A인벤에 B의 아이템 추가
        if (IncreaseItem(conn, std::to_string(session->a_id), item) != 0)
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] IncreaseItem failed", __FUNCTION__, __LINE__);
            goto err;
        }

    }

err:
    if (result != 0)
    {
        mysql_query(conn, "ROLLBACK");
        return -1;
    }
    else
    {
        mysql_query(conn, "COMMIT");

        //4. 교환세션 삭제
        DeleteTradeSession(session); 
    }

    return 0;
}

void TradeService::CreateTradeSession(Player *a_player, Player *b_player)
{
    if (a_player == nullptr || b_player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] a_player or b_player is nullptr", __FUNCTION__, __LINE__);
        return;
    }

    TradeSession *session = new TradeSession();

    session->a_id = a_player->GetId();
    session->b_id = b_player->GetId();
    m_sessions[a_player->GetId()] = session;
    m_sessions[b_player->GetId()] = session;
}

void TradeService::DeleteTradeSession(TradeSession *session)
{
    if (session == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session is nullptr", __FUNCTION__, __LINE__);
        return;
    }
    m_sessions.erase(session->a_id);
    m_sessions.erase(session->b_id);

    delete session;
}

//당장 불필요
TradeSession *TradeService::GetTradeSession(Player *player)
{
    if (player == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player is nullptr", __FUNCTION__, __LINE__);
        return nullptr;
    }

    auto it = m_sessions.find(player->GetId());
    if (it == m_sessions.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] player_id(%d) TradeSession not found", __FUNCTION__, __LINE__, player->GetId());
        return nullptr;
    }

    return it->second;
}

int TradeService::DecreaseItem(MYSQL *conn, const std::string &char_id, const TradeItem &item)
{
    std::string query;
    int result = 0;

    //UPDATE (수량 감소)
    query = "UPDATE character_inventory SET item_count = item_count - " + std::to_string(item.amount) + " WHERE char_id = " + char_id + " AND item_id = " + item.id;

    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] query: %s", __FUNCTION__, __LINE__, query.c_str());
    result = mysql_query(conn, query.c_str());
    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] UPDATE FAIL: %s", __FUNCTION__, __LINE__, mysql_error(conn));
        return -1;
    }

    //DELETE(수량 0 일경우)
    query = "DELETE FROM character_inventory WHERE char_id = " + char_id + " AND item_id = " + item.id + " AND item_count <= 0";
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] query: %s", __FUNCTION__, __LINE__, query.c_str());
    result = mysql_query(conn, query.c_str());
    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] DELETE FAIL: %s", __FUNCTION__, __LINE__, mysql_error(conn));
        return -1;
    }

    return 0;
}

int TradeService::IncreaseItem(MYSQL *conn, const std::string &char_id, const TradeItem &item)
{
    std::string query;
    int result = 0;

    //INSERT (아이템이 없을 경우) 있으면 UPDATE
    query = "INSERT INTO character_inventory (char_id, inventory_type, slot_pos, item_id, item_count) VALUES (" + char_id + "," +  item.type + ", 9," + item.id + ", " + std::to_string(item.amount) + ") ON DUPLICATE KEY UPDATE item_count = item_count + " + std::to_string(item.amount);

    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] query: %s", __FUNCTION__, __LINE__, query.c_str());
    result = mysql_query(conn, query.c_str());
    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] INSERT FAIL: %s", __FUNCTION__, __LINE__, mysql_error(conn));
        return -1;
    }

    return 0;
}
