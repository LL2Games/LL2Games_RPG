#include "Inventory.h"


Inventory::Inventory(InventoryMetaInfo& inventoryMetaInfo)
{
    m_inventoryType = inventoryMetaInfo.inventoryType;
    m_maxSlot = inventoryMetaInfo.max_slots;
    m_current_slot_size = inventoryMetaInfo.currnet_slots_size;

    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] m_inventoryType = %d",__FUNCTION__, __LINE__, m_inventoryType);
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] m_maxSlot = %d",__FUNCTION__, __LINE__, m_maxSlot);
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] m_current_slot_size = %d",__FUNCTION__, __LINE__, m_current_slot_size);

    m_slots.reserve(m_maxSlot);
    for(int i = 0; i < m_maxSlot; i++)
    {
        InventorySlot slot{};
        slot.slotPos = i;
        slot.isEnable = (i <= m_current_slot_size);
        slot.inventoryType = m_inventoryType;
        slot.itemId = 0;
        slot.itemCount = 0;
        m_slots.emplace(i, slot);
    }
    K_slog_trace(K_SLOG_DEBUG, "Inventory Init Success");
}



// DB에서 아이템 조회 후 저장하는 함수
bool Inventory::SetSlotItem(int slotPos, int itemId, int count)
{
    InventorySlot items;

    items.inventoryType = m_inventoryType;
    items.slotPos = slotPos;
    items.itemId = itemId;
    items.itemCount = count;

    m_slots[slotPos] = items;

    return true;
}

bool Inventory::AddItem(int itemId, int count)
{
    (void) itemId;
    (void) count;
    return true;
}

// 아이템 사용 시 아이템 개수 변경
bool Inventory::RemoveItemBySlot(int slotPos, int itemId, int count)
{
    
    K_slog_trace(K_SLOG_DEBUG, "[%s : %s][%d] itemData UseCount [%d]", __FILE__, __FUNCTION__, __LINE__, count);
    auto it = m_slots.find(slotPos);

    if(it == m_slots.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 플레이어가 가지고 있지 않은 아이템입니다.", __FUNCTION__, __LINE__);
        return false;
    }

    if(it->second.itemId != itemId)
    {
        K_slog_trace(K_SLOG_ERROR, "[%d][%s] 아이템 ID가 다릅니다. have_itemID=%d 요청 아이템 ID=%d",__FUNCTION__, __LINE__, it->second.itemId, itemId);
        return false;
    }
    
     if (it->second.itemCount < count)
    {
        K_slog_trace(K_SLOG_ERROR, "[%d][%s] 아이템 개수가 충분하지 않습니다. have=%d need=%d",__FUNCTION__, __LINE__, it->second.itemCount, count);
        return false;
    }

    it->second.itemCount -= count;

    if(it->second.itemCount == 0)
    {
        m_slots.erase(it);
    }

    return true;

}

bool Inventory::HasItemBySlot(int slotPos, int itemId, int count) const
{
    K_slog_trace(K_SLOG_DEBUG, "[%s : %s][%d] 아이템 사용 개수 [%d]", __FILE__, __FUNCTION__, __LINE__, count);
    auto it = m_slots.find(slotPos);

    if(it == m_slots.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d]  플레이어가 가지고 있지 않은 아이템입니다.", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if(it->second.itemId != itemId)
    {
         K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] 아이템 ID가 다릅니다.", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if(it->second.itemCount < count)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s : %s][%d] 아이템 개수가 충분하지 않습니다. inventoryType=%d slotPos=%d have=%d need=%d"
            ,__FILE__, __FUNCTION__, __LINE__, it->second.inventoryType, it->second.slotPos, it->second.itemCount, count);
        return false;
    }

    return true;
}

bool Inventory::HasItemBySlot(int slotPos, int itemId) const
{
   
    auto it = m_slots.find(slotPos);

    if(it == m_slots.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 플레이어가 가지고 있지 않은 아이템입니다.", __FUNCTION__,__LINE__);
        return false;
    }

    if(it->second.itemId != itemId)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] [%d][%d] 아이템 ID가 다릅니다.",__FUNCTION__, __LINE__,it->second.itemId, itemId);
        return false;
    }

    return true;
}

InventorySlot* Inventory::FindSlot(int slotPos)
{
    auto it = m_slots.find(slotPos);

    if (it != m_slots.end())
    {
    	return &(it->second);
    }

    return nullptr;
}

int Inventory::GetItemCount(int slotPos, int itemId) const
{

    auto it = m_slots.find(slotPos);

    if(it == m_slots.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 플레이어가 가지고 있지 않은 아이템입니다.", __FUNCTION__,__LINE__);
        return 0;
    }

    if(it->second.itemId != itemId)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 아이템 ID가 다릅니다.",__FUNCTION__, __LINE__);
        return 0;
    }

    return it->second.itemCount;
}

std::vector<InventoryItemInfo> Inventory::MakeItemInfos() const
{
    std::vector<InventoryItemInfo> inventoryItemInfos;

    for(auto [slotpos, item] : m_slots)
    {
        InventoryItemInfo iteminfo;

        iteminfo.inventoryType =m_inventoryType;
        iteminfo.slotPos = item.slotPos;
        iteminfo.itemCount =item.itemCount;
        iteminfo.itemId = item.itemId;

        inventoryItemInfos.push_back(iteminfo);
    }
    return inventoryItemInfos;
}