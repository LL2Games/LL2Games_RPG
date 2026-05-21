#include "QuickSlotManager.h"


bool QuickSlotManager::SetSlot(const QuickSlotData& data)
{
    auto slot = m_quickSlots.find(data.slot_index);

    if(slot != m_quickSlots.end())
    {
        return false;
    }
    K_slog_trace(K_SLOG_DEBUG, "[%s : %s][%d] SetSlot is complete\n", __FILE__, __FUNCTION__, __LINE__);
    m_quickSlots[data.slot_index] = data;
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