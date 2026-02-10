#include "PlayerManager.h"

PlayerManager::PlayerManager()
{
}

PlayerManager::~PlayerManager()
{
}


bool PlayerManager::AddPlayer(std::unique_ptr<Player> player)
 {
    int playerId = player->GetId();

    if(m_players.find(playerId) != m_players.end())
    {
            return false; 
    }
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] Player character_id [%d]", __FUNCTION__ , __LINE__, player->GetId());
    K_slog_trace(K_SLOG_TRACE, " [%s][%d] Player character_id [%s]", __FUNCTION__ , __LINE__, player->GetName().c_str());

    m_players[playerId] = move(player);
     return true;
}




Player* PlayerManager::GetPlayer(int playerID)
{
    auto it = m_players.find(playerID);

    if(it == m_players.end()) return nullptr;

    return it->second.get();
}