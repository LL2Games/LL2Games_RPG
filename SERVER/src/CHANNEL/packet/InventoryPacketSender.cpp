#include "InventoryPacketSender.h"
#include "ChannelSession.h"
#include "K_slog.h"

void InventoryPacketSender::SendInventoryMeta(Player* player)
{
    auto session = player->GetSession();

    if(!session)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session이 nullptr입니다.", __FUNCTION__, __LINE__);
        return;
    }

    std::vector<std::string> payload;

    auto inventory = player->GetInventoryManager();

    // inventory size 처음에 넣어줌 클라이언트에서 받을 때 개수를 통해서 검증 시도 및 정확한 개수를 받기 위해서
    payload.push_back(std::to_string(inventory->GetAllMetaInfos().size()));

    for(auto metaInfos : inventory->GetAllMetaInfos())
    {
        payload.push_back(std::to_string(metaInfos.inventoryType));
        payload.push_back(std::to_string(metaInfos.max_slots));
        payload.push_back(std::to_string(metaInfos.currnet_slots_size));
    }

    session->Send(PKT_INVENTORY_META_INFO, payload);
    
}

void InventoryPacketSender::SendInventoryItems(Player* player)
{
    auto session = player->GetSession();

    if(!session)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] session이 nullptr입니다.", __FUNCTION__, __LINE__);
        return;
    }

    std::vector<std::string> payload;

    auto inventory = player->GetInventoryManager();
    
    // 아이템 개수 처음에 넣어줌 클라이언트에서 받을 때 검증과 정확한 개수의 아이템을 받기 위해서
    payload.push_back(std::to_string(inventory->GetAllItemInfos().size()));

    for(auto itemInfos : inventory->GetAllItemInfos())
    {
        payload.push_back(std::to_string(itemInfos.inventoryType));
        payload.push_back(std::to_string(itemInfos.itemId));
        payload.push_back(std::to_string(itemInfos.itemCount));
        payload.push_back(std::to_string(itemInfos.slotPos));
    }

    

    session->Send(PKT_INVENTORY_ITEM_INFO, payload);
}