#include "ItemManager.h"

#define ITEM_PATH "../src/CHANNEL/data/Items/"
namespace fs = std::filesystem;


ItemManager *ItemManager::m_instance =nullptr;

bool ItemManager::Init()
{
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] ItemManagerInit Start",__FUNCTION__, __LINE__);
    if(!PreLoadAll()) return false;
    return true;
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

        ItemInitData* itemData = new ItemInitData();
        if (!LoadJsonFile(entry.path().string(), *itemData))
        {
            delete itemData;
            return false;
        }

        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] ITEM ID [%d]",__FUNCTION__, __LINE__, item_id);
        K_slog_trace(K_SLOG_DEBUG, "[%s][%d] ITEM Manager Pointer [%p]",__FUNCTION__, __LINE__, this);
        m_item_initData.emplace(item_id, itemData);
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

        itemData.item_id    = j.at("item_id").get<int>();
        itemData.name       = j.at("name").get<std::string>();
        itemData.type       = j.at("type").get<std::string>();
        itemData.stackable  = j.at("stackable").get<bool>();
        itemData.max_stack  = j.at("max_stack").get<int>();
        itemData.sell_price = j.at("sell_price").get<int>();
    
    if (itemData.type == "consumable" && j.contains("use_effect"))
    {
        const auto& ue = j.at("use_effect").at(0);
        UseEffect effect;
        effect.hp_restore  = ue.at("hp_restore").get<int>();
        effect.mp_restore  = ue.at("mp_restore").get<int>();
        effect.cooldown_ms = ue.at("cooldown_ms").get<int>();
        itemData.use_effect = effect;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] ItemID [%d]",__FUNCTION__, __LINE__, itemData.item_id);

    return true;
}


const ItemInitData* ItemManager::Find(int item_ID)
{
    auto it = m_item_initData.find(item_ID);

    if(it == m_item_initData.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] ItemID CANT FIND [%d]",__FUNCTION__, __LINE__, item_ID);
        return nullptr;
    }

    return it->second;


}

ItemManager* ItemManager::GetInstance()
{
    if (m_instance == nullptr)
        m_instance = new ItemManager();
    return m_instance;
}
