#pragma once

enum class QuickSlotType
{
    None = 0,
    Skill,
    Item,
    UI,
    Emote
};

struct QuickSlotData
{
    int slot_index;
    QuickSlotType type;
    int ref_id;   // skill_id 또는 item_id 등
    int inventory_type;
    int inventory_slotPos;
};


namespace QuickSlot
{
    inline QuickSlotType SetSlotType(int type)
    {
        switch(type)
        {
            case 1:
                return QuickSlotType::Skill;
            case 2:
                return QuickSlotType::Item;
            case 3:
                return QuickSlotType::UI;
            case 4:
                return QuickSlotType::Emote;
            default:
                return QuickSlotType::None;
        }
    }

}