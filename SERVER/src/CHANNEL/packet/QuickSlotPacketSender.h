#pragma once
#include "common.h"
#include "Player.h"
#include "Slot_Info.h"
class QuickSlotPacketSender
{
public:
    static void SendQuickSlotList(Player* player);
    static void SendQuickSlotSet(Player* player, std::vector<QuickSlotData>& datas);
private:
};