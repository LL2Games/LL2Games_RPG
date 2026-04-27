#include "common.h"
#include "Player.h"

class PlayerPacketSender
{
public:
    static void SendPlayerInfo(Player* player);
    static void SendPlayerStat(Player* player);
    static void SendPlayerSkillList(Player* player);
private:


};