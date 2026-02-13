#include "common.h"
#include "PlayerManager.h"
#include "MapManager.h"

class MapService
{
public:
    MapService(PlayerManager& playerManager, MapManager& mapManager);
    ~MapService(){};

    // player�� ��Ż�� ���� map�� ������ �� ����Ǵ� �Լ�
    int HandlePortalUse(int playerID, int mapID);

private:
    PlayerManager& m_playerManager;
    MapManager& m_MapManger;
};