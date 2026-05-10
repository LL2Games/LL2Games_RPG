#pragma once
#include "common.h"
#include "Inventory_Info.h"

class Inventory
{

public:
    Inventory() = default;
    ~Inventory(){};

    Inventory(InventoryMetaInfo& inventoryMetaInfo);

    bool SetSlotItem(int slotPos, int itemId, int count);
    bool RemoveItemBySlot(int slotPos, int itemId,int count);
    bool HasItemBySlot(int slotPos, int itemId, int count) const;
    bool HasItemBySlot(int slotPos, int itemId) const;

    InventorySlot* FindSlot(int slotPos);

    bool AddItem(int itemId, int count);    

    const std::map<int, InventorySlot>& GetAllItems() const;

    int GetItemCount(int slotPos, int itemId) const;
    void Clear();

public:
    int GetInventoryType() {return m_inventoryType;}
    int GetMaxSlotSize()   {return m_maxSlot;}
    int GetCurrentSlotSize() {return m_current_slot_size;}

    std::vector<InventoryItemInfo> MakeItemInfos() const;

private:
    // key : 인벤토리 타입, 슬롯 위치, value : 인벤토리 타입, 슬롯 위치, 아이템 아이디, 개수
    std::unordered_map<int, InventorySlot> m_slots;
    int m_inventoryType;
    int m_maxSlot;
    int m_current_slot_size;
};      