#include "ItemService.h"
#include "ChannelSession.h"
#include "PacketDTO.h"

ItemService::ItemService()
{
    m_item_manager = ItemManager::GetInstance();
}


int ItemService::HandleUseItem(Player* player, int itemId, int useCount, UseItemResult& result)
{
    int is_use; 
    if(!player->CanUseItem(itemId, useCount))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s:%d] [%d] 아이템 사용 불가\n", __FUNCTION__, __LINE__, itemId);
        return EXIT_FAILURE;
    }

    is_use = player->UseItem(itemId, useCount);

    if(!is_use)
    {
        result.result = false;
        return false;
    }

    // 해당 플레이어한테 체력 변경 전달 및 아이템 사용 동기화
    // is_use가 현재 bool 형인데 나중에 err_code 보내는 것으로 변경 해야함 
    result.result = is_use;
    result.hp = player->GetCurrHP();
    result.mp = player->GetCurrMP();
    result.item_id = itemId;
    result.used_count = useCount;
    result.remain_count = player->GetItemCount(itemId);

    return EXIT_SUCCESS;
}

