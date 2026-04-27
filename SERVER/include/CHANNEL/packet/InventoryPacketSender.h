#include "common.h"
#include "Player.h"

class InventoryPacketSender
{
public:
    static void SendInventoryMeta(Player* player);
    static void SendInventoryItems(Player* player);

private:

};