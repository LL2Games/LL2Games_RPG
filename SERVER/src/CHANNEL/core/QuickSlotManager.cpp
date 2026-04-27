#include "QuickSlotManager.h"


bool QuickSlotManager::SetSlot(int slotIndex, const QuickSlotData& data)
{
    auto slot = m_quickSlots.find(slotIndex);

    if(slot != m_quickSlots.end())
    {
        return false;
    }

    m_quickSlots[slotIndex] = data;
    return true;
}
    
void QuickSlotManager::RemoveSlot(int slotIndex)
{
    auto slot = m_quickSlots.find(slotIndex);

    if(slot != m_quickSlots.end())
    {
        slot->second =  QuickSlotData{}; 
    }
}

 const QuickSlotData* QuickSlotManager::GetSlot(int slotIndex) const
 {
    auto slot = m_quickSlots.find(slotIndex);

    if(slot == m_quickSlots.end())
    {
        return nullptr;
    }

    return &(slot->second);
 }

 const std::vector<QuickSlotData> QuickSlotManager::GetSlotList() const
 {
    std::vector<QuickSlotData> quickSlotDatas;

    quickSlotDatas.reserve(m_quickSlots.size());

    for(auto [id, slotData] : m_quickSlots)
    {
        quickSlotDatas.push_back(slotData);
    }
    return quickSlotDatas;
 }