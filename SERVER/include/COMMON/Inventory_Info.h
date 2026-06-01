#pragma once
#include "common.h"
#include <optional>
struct Str_UseItem
{
    std::string str_inventoryType;
    std::string str_slotPos;
    std::string str_itemId;
    std::string str_useCount;
};

struct UseEffect
{
    int hp_restore = 0;
    int mp_restore = 0;
    int cooldown_ms = 0;
};


struct ItemInitData
{
    int item_id = 0;
    std::string name;
    std::string type;  // "consumable" / "equipment" /
    bool stackable = false;
    int max_stack = 1;
    int sell_price = 0;

    std::optional<UseEffect> use_effect; // consumable에만 존재
};


struct UseItem
{
    int inventoryType = 0;
    int slotPos = 0;
    int itemId = 0;
    int useCount = 0;
};

struct MoveItem
{
    int inventorytype = 0;
    int fromSlotPos = 0;
    int toSlotPos = 0;
};

struct AddItemData
{
    int itemId = 0;
    int count = 0;
    bool stackable = false;
    int max_stack = 1;
};

struct InventorySlot
{
    int inventoryType = 0;
    int slotPos = 0;
    int itemId = 0;
    int itemCount = 0;
    bool isEnable = true;
};

struct InventorySlotUpdate
{
    int slotPos = 0;
    int itemId= 0;
    int itemCount = 0;
};

struct MoveItemResult
{
    bool result = false;
    int inventoryType = 0;
    std::vector<InventorySlotUpdate> updateSlots;
};

struct AddItemResult
{
    int inventoryType = -1;
    int slotPos = -1;
    int itemId = 0;
    int itemCount = 0; 
};

struct InventoryMetaInfo
{
    int inventoryType = 0;
    int max_slots = 0;
    int currnet_slots_size = 0;
};

struct InventoryItemInfo
{
    int inventoryType = 0;
    int slotPos = 0;
    int itemId = 0;
    int itemCount = 0;
};

namespace inven
{
    enum eInventoryType
    {
        Equip = 0,
        Consume = 1,
        Etc = 2,
        Setup = 3,
        Cash = 4,
        Invalid = -1
    };

    inline int ConvertItemTypeToInventoryType(const std::string& type)
    {
        if (type == "equip")
            return eInventoryType::Equip;

        if (type == "consumable")
            return eInventoryType::Consume;

        if (type == "etc")
            return eInventoryType::Etc;

        if (type == "setup")
            return eInventoryType::Setup;

        if (type == "cash")
            return eInventoryType::Cash;
        return -1;
    }
}