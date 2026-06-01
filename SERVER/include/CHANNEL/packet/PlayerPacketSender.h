#include "common.h"
#include "Player.h"
#include "StatInfoPacket.h"
class PlayerPacketSender
{
public:
    static void SendPlayerInfo(Player* player);
    static void SendPlayerStat(Player* player);
    static void SendPlayerSkillList(Player* player);
    static void SendPlayersMove(Player* sender, Vec2 pos, float speed, std::unordered_map<int, Player*>& playerList);
    static void SendPlayerOnDamaged(Player* Defender, PlayerHitResult result, std::unordered_map<int, Player*>& playerList);
    static void SendExpGain(Player* player, const ExpResult& expResult);
private:


};