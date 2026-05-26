#pragma once
#include "common.h"
#include "Slot_Info.h"

class QuickSlotManager
{
public:
    void Init();        
    std::vector<QuickSlotData> SetSlot(const QuickSlotData& data);  
    void RemoveSlot(int slotIndex);

    bool IsSameTarget(const QuickSlotData& a, const QuickSlotData& b);
    void ClearSlot(int slotIndex);
public:
    const QuickSlotData* GetSlot(int slotIndex) const;
    const std::vector<QuickSlotData> GetSlotList() const;
private:
    std::unordered_map<int, QuickSlotData> m_quickSlots;
    static constexpr int m_maxSlotCount = 32;
};