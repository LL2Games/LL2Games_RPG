#include "common.h"
#include "PlayerManager.h"
#include "MapManager.h"

class MapService
{
public:
    MapService(PlayerManager& playerManager, MapManager& mapManager);
    ~MapService(){};

    // player가 포탈을 통해 map에 들어왔을 때 수행되는 함수
    int HandlePortalUse(int playerID, int mapID);

private:
    PlayerManager& m_playerManager;
    MapManager& m_MapManger;
};