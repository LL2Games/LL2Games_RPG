#pragma once
#include "common.h"
#include "Inventory.h"

class InventoryManager
{
public:
    
	bool CreateInventory(int inventoryType, int maxSlot, int currnet_slot_size);
	void EnsureInventory(int inventoryType, int maxSlot, int currnet_slot_size);

	
	Inventory* GetInventory(int inventoryType);
	const Inventory* GetInventory(int inventoryType) const;

	InventorySlot* FindSlot(int inventoryType, int slotPos);
	//const InventorySlot* FindSlot(int inventoryType, int slotPos) const;

	void Clear();
private:
    std::unordered_map<int, Inventory> m_inventories;
};