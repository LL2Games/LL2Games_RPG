#include "ItemManager.h"

#define ITEM_PATH "../src/CHANNEL/data/Items/"
namespace fs = std::filesystem;




void ItemManager::Init()
{
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] ItemManagerInit Start",__FUNCTION__, __LINE__);
    PreLoadAll();
}


bool ItemManager::PreLoadAll()
{
    for (const auto& entry : fs::recursive_directory_iterator(ITEM_PATH))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".json") continue;

        // 파일명에서 id 추출 (예: 2000000.json)
        int item_id = 0;
        try {
            item_id = std::stoi(entry.path().stem().string());
        } catch (...) {
            continue;
        }

        if (m_item_initData.find(item_id) != m_item_initData.end())
            continue;

        ItemInitData itemData;
        if (!LoadJsonFile(entry.path().string(), itemData))
            return false;

        m_item_initData.emplace(item_id, std::move(itemData));
    }
    return true;
}

bool ItemManager::LoadJsonFile(const std::string& path, ItemInitData& itemData)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE",__FUNCTION__, __LINE__, path.c_str());
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error&) {
        return false;
    }
    if (j.is_null()) return false;

    itemData.item_id    = j.value("item_id", 0);
    itemData.name       = j.value("name", "");
    itemData.type       = j.value("type", "");
    itemData.stackable  = j.value("stackable", false);
    itemData.max_stack  = j.value("max_stack", itemData.stackable ? 100 : 1);
    itemData.sell_price = j.value("sell_price", 0);

    if (itemData.type == "consumable" && j.contains("use_effect"))
    {
        const auto& ue = j["use_effect"];
        itemData.use_effect->hp_restore  = ue.value("hp_restore", 0);
        itemData.use_effect->mp_restore  = ue.value("mp_restore", 0);
        itemData.use_effect->cooldown_ms = ue.value("cooldown_ms", 0);
    }

     K_slog_trace(K_SLOG_TRACE, "[%s][%d] ItemID [%d]",__FUNCTION__, __LINE__, itemData.item_id);

    return true;
}