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
    if(player == nullptr) return -1;

    map = m_MapManger.GetOrCreate(mapID);

    map->OnEnter(playerID, player);

    return EXIT_SUCCESS;
}