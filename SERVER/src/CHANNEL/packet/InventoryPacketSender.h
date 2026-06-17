#include "common.h"
#include "Player.h"
#include "Inventory_Info.h"

class InventoryPacketSender
{
public:
    static void SendInventoryMeta(Player* player);
    static void SendInventoryItems(Player* player);
    static void SendInventoryMoveItem(ChannelSession* session, bool result, int inventoryType, const std::vector<InventorySlotUpdate>& updatedSlots, const std::string& errMsg);
private:

};