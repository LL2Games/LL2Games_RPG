#include "Inventory.h"


Inventory::Inventory()
{

}

// DB에서 아이템 조회 후 저장하는 함수
bool Inventory::SetSlotItem(int inventoryType, int slotPos, int itemId, int count)
{
    InventorySlot items;

    items.inventoryType = inventoryType;
    items.slotPos = slotPos;
    items.itemId = itemId;
    items.itemCount = count;

    std::pair<int, int> key = {inventoryType, slotPos};
    m_slots[key] = items;

    return true;
}

bool Inventory::AddItem(int itemId, int count)
{
    (void) itemId;
    (void) count;
    return true;
}

// 아이템 사용 시 아이템 개수 변경
bool Inventory::RemoveItemBySlot(int inventoryType, int slotPos, int itemId,int count)
{
    std::pair<int,int> key = {inventoryType, slotPos};

    auto it = m_slots.find(key);

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
        K_slog_trace(K_SLOG_ERROR, "[%d][%s] 아이템 개수가 충분하지 않습니다. have=%d need=%d",__FUNCTION__, __LINE__,inventoryType, slotPos,it->second.itemCount, count);
        return false;
    }

    it->second.itemCount -= count;

    if(it->second.itemCount == 0)
    {
        m_slots.erase(it);
    }

    return true;

}

bool Inventory::HasItemBySlot(int inventoryType, int slotPos, int itemId, int count) const
{
    std::pair<int, int> key = {inventoryType, slotPos};

    auto it = m_slots.find(key);

    if(it == m_slots.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 플레이어가 가지고 있지 않은 아이템입니다.", __FUNCTION__,__LINE__);
        return false;
    }

    if(it->second.itemId != itemId)
    {
         K_slog_trace(K_SLOG_ERROR, "[%s][%d] 아이템 ID가 다릅니다.",__FUNCTION__, __LINE__,inventoryType, slotPos,it->second.itemCount, count);
        return false;
    }

    if(it->second.itemCount < count)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 아이템 개수가 충분하지 않습니다. inventoryType=%d slotPos=%d have=%d need=%d",__FUNCTION__, __LINE__,inventoryType, slotPos,it->second.itemCount, count);
        return false;
    }

    return true;
}

bool Inventory::HasItemBySlot(int inventoryType, int slotPos, int itemId) const
{
    std::pair<int, int> key = {inventoryType, slotPos};

    auto it = m_slots.find(key);

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

int Inventory::GetItemCount(int inventoryType, int slotPos, int itemId) const
{
    std::pair<int, int> key = {inventoryType, slotPos};

    auto it = m_slots.find(key);

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