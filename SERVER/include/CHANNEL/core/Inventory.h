#pragma once
#include "common.h"
#include "Inventory_Info.h"

class Inventory
{

public:
    Inventory();
    ~Inventory(){};

    bool SetSlotItem(int inventoryType, int slotPos, int itemId, int count);
    bool RemoveItemBySlot(int inventoryType, int slotPos, int itemId,int count);
    bool HasItemBySlot(int inventoryType, int slotPos, int itemId, int count) const;
    bool HasItemBySlot(int inventoryType, int slotPos, int itemId) const;

    bool AddItem(int itemId, int count);    

    const std::map<std::pair<int, int>, InventorySlot>& GetAllItems() const;
    void Clear();

public:
    int GetItemCount(int inventoryType, int slotPos, int itemId) const;

private:
    // key : 인벤토리 타입, 슬롯 위치, value : 인벤토리 타입, 슬롯 위치, 아이템 아이디, 개수
    std::map<std::pair<int, int>, InventorySlot> m_slots;
};      