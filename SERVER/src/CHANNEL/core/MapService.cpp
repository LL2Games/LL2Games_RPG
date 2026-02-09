#include "MapService.h"
#include "Player.h"
#include "MapInstance.h"


MapService::MapService(PlayerManager& playermanager, MapManager& mapManager) : m_playerManager(playermanager), m_MapManger(mapManager)
{

}

int MapService::HandlePortalUse(int playerID, int mapID)
{
    Player* player;
    MapInstance* map;
    player = m_playerManager.GetPlayer(playerID);
    if(player == nullptr) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] Player is nullptr", __FILE__, __FUNCTION__, __LINE__);
        return EXIT_FAILURE;
    }
    // 맵 정보 로드 및 생성
    map = m_MapManger.GetOrCreate(mapID);

    // 플레이어 현재 맵 설정
    player->SetCurrentMap(map);

    // 맵 Enter 로직 수행
    map->OnEnter(playerID, player);

    return EXIT_SUCCESS;
}