#pragma once
#include "common.h"
#include "Slot_Info.h"

class QuickSlotManager
{
public:
    bool SetSlot(int slotIndex, const QuickSlotData& data);
    
    void RemoveSlot(int slotIndex);

public:
    const QuickSlotData* GetSlot(int slotIndex) const;
    const std::vector<QuickSlotData> GetSlotList() const;
private:
    std::unordered_map<int, QuickSlotData> m_quickSlots;

};