#include "QuickSlotPacketSender.h"
#include "ChannelSession.h"


void QuickSlotPacketSender::SendQuickSlotList(Player* player)
{
    auto session = player->GetSession();

    if(!session) 
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session이 nullptr입니다.", __FUNCTION__, __LINE__);
        return;
    }

    auto quickSlotManager = player->GetQuickSlotManager();
    auto quickSlotList = quickSlotManager->GetSlotList();
    std::vector<std::string> payload;

    payload.push_back(std::to_string(quickSlotList.size()));
    // 퀵슬롯 정보 전달
    for(auto quickSlot : quickSlotList)
    {
        payload.push_back(std::to_string(quickSlot.slot_index));
        payload.push_back(std::to_string(static_cast<int>(quickSlot.type)));
        payload.push_back(std::to_string(quickSlot.ref_id));
        payload.push_back(std::to_string(quickSlot.inventory_type));
        payload.push_back(std::to_string(quickSlot.inventory_slotPos));
    }

    session->Send(PKT_QUICKSLOT_LIST, payload);

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] SendQuickSlotList Send Success.", __FUNCTION__, __LINE__);
}
