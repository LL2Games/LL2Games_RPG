#pragma once
#include "common.h"
#include "Inventory.h"

class InventoryManager
{
public:
    
	bool CreateInventory(InventoryMetaInfo& inventoryMetaInfo);
	void EnsureInventory(InventoryMetaInfo& inventoryMetaInfo);

	
	Inventory* GetInventory(int inventoryType);
	const Inventory* GetInventory(int inventoryType) const;

	InventorySlot* FindSlot(int inventoryType, int slotPos);
	//const InventorySlot* FindSlot(int inventoryType, int slotPos) const;

	std::vector<InventoryMetaInfo> GetAllMetaInfos() const;
	std::vector<InventoryItemInfo> GetAllItemInfos() const;

	void Clear();
private:
    std::unordered_map<int, Inventory> m_inventories;

};