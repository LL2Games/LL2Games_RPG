#pragma once
#include "common.h"
struct Str_UseItem
{
    std::string str_inventoryType;
    std::string str_slotPos;
    std::string str_itemId;
    std::string str_useCount;
};

struct UseItem
{
    int inventoryType = 0;
    int slotPos = 0;
    int itemId = 0;
    int useCount = 0;
};

struct InventorySlot
{
    int inventoryType = 0;
    int slotPos = 0;
    int itemId = 0;
    int itemCount = 0;
    bool isEnable = true;
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
