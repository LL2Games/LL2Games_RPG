#include "InventroyManager.h"
#include "Inventory_Info.h"

bool InventoryManager::CreateInventory(int inventoryType, int maxSlot, int currnet_slot_size)
{
    auto it = m_inventories.find(inventoryType);
    if(it != m_inventories.end())
    {
        return false;
    }
    
    m_inventories.emplace(inventoryType, Inventory(inventoryType, maxSlot, currnet_slot_size));

    return true;
}

void InventoryManager::EnsureInventory(int inventoryType, int maxSlot, int currnet_slot_size)
{
    auto it = m_inventories.find(inventoryType);
    if (it == m_inventories.end())
    {
    	m_inventories.emplace(inventoryType, Inventory(inventoryType, maxSlot, currnet_slot_size));
    }
}

Inventory* InventoryManager::GetInventory(int inventoryType)
{
    auto it = m_inventories.find(inventoryType);
    if (it == m_inventories.end())
    {
    	return nullptr;
    }

    return &(it->second);
}

InventorySlot* InventoryManager::FindSlot(int inventoryType, int slotPos)
{
    auto it = m_inventories.find(inventoryType);

    if (it == m_inventories.end())
    {
    	return nullptr;
    }
    return it->second.FindSlot(slotPos);
}

void InventoryManager::Clear()
{
    m_inventories.clear();
}


