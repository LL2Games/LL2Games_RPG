#include "DropManager.h"

#define DROPDATA_PATH "../src/CHANNEL/data/Drops/"

DropManager *DropManager::m_instance = nullptr;

DropManager::DropManager() : m_gen(std::random_device{}())
{

}

DropManager *DropManager::GetInstance()
{
     if(m_instance == nullptr)
    {
        m_instance = new DropManager();
        //if(m_instance->Init() != EXIT_SUCCESS)
        //{
        //    delete m_instance;
        //    m_instance = nullptr;
        //}
    }
    return m_instance;
}

bool DropManager::Init()
{
    if(!PreLoadAll()) return false;
    return true;
}

bool DropManager::PreLoadAll()
{
    for(const auto &entry : fs::directory_iterator(DROPDATA_PATH))
    {
        if(!entry.is_regular_file())
            continue;
        if(entry.path().extension() != ".json")
            continue;

        std::string filename = entry.path().stem().string();

        if(filename == "monster_drop_groups_common")
        {
            bool is_commonLoad = LoadCommonDropFile(entry.path());
            if(!is_commonLoad) return false;
        }else if(filename == "monster_drop_groups_unique")
        {
            bool is_uniqueLoad = LoadUniqueDropFile(entry.path());
            if(!is_uniqueLoad) return false;
        }        
    }
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] DropManager PreLoadAll Success", __FUNCTION__, __LINE__);
    return true;
}

bool DropManager::LoadCommonDropFile(const fs::path& path)
{
   
    std::ifstream file(path);

    if(!file.is_open())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return false;
    }

    nlohmann::json j;
  
    try
    {
        file >> j;
    }
    catch (const std::exception& e)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] JSON PARSE ERROR [%s] : %s",__FUNCTION__, __LINE__, path.string().c_str(), e.what());
        return false;
    }

    if(!j.contains("drop_groups") || !j["drop_groups"].is_object())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] INVALID drop_groups [%s]",__FUNCTION__, __LINE__, path.string().c_str());
        return false;
    }

    const auto& groups = j["drop_groups"];

    for(auto it = groups.begin(); it != groups.end(); ++it)
    {
        const std::string groupId = it.key();
        const auto& groupJson = it.value();

        DropGroup group;
        group.groupId = groupId;
        group.minDrop = groupJson.value("min_drop",0);
        group.maxDrop = groupJson.value("max_drop",0);
        group.allowDuplicate = groupJson.value("allow_duplicate", false);

        if (!groupJson.contains("entries") || !groupJson["entries"].is_array())
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] INVALID entries in group [%s]",__FUNCTION__, __LINE__, groupId.c_str());
            return false;
        }

        for(const auto& entryJson : groupJson["entries"])
        {
            DropEntry entry;

            std::string type = entryJson.value("type", "");
            if(type == "item")
            {
                entry.type = DropType::Item;
                entry.itemId = entryJson.value("item_id", 0);
                entry.weight = entryJson.value("weight",0);
                entry.minCount = entryJson.value("min_count",0);
                entry.maxCount = entryJson.value("max_count",0);
                entry.tag = entryJson.value("tag", "");
            }
            else
            {
                K_slog_trace(K_SLOG_ERROR, "[%s][%d] UNKNOWN entry type in group [%s]",__FUNCTION__, __LINE__, groupId.c_str());
                return false;
            }
            group.entries.push_back(entry);
        }
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] common groupId [%s]",__FUNCTION__, __LINE__, group.groupId.c_str());
        m_commonGroups[group.groupId] = group;
    }   
    return true;

}


bool DropManager::LoadUniqueDropFile(const fs::path& path)
{
    std::ifstream file(path);

    if(!file.is_open())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return false;
    }

    nlohmann::json j;

    try
    {
        file >> j;
    }
    catch (const std::exception& e)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] JSON PARSE ERROR [%s] : %s",__FUNCTION__, __LINE__, path.string().c_str(), e.what());
        return false;
    }

    if(!j.contains("drop_groups") || !j["drop_groups"].is_object())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] INVALID drop_groups [%s]",__FUNCTION__, __LINE__, path.string().c_str());
        return false;
    }

    const auto& groups = j["drop_groups"];

    for(auto it = groups.begin(); it != groups.end(); ++it)
    {
        const std::string groupId = it.key();
        const auto& groupJson = it.value();

        DropGroup group;
        group.groupId = groupId;
        group.minDrop = groupJson.value("min_drop",0);
        group.maxDrop = groupJson.value("max_drop",0);
        group.allowDuplicate = groupJson.value("allow_duplicate", false);

        if (!groupJson.contains("entries") || !groupJson["entries"].is_array())
        {
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] INVALID entries in group [%s]",__FUNCTION__, __LINE__, groupId.c_str());
            return false;
        }

        for(const auto& entryJson : groupJson["entries"])
        {
            DropEntry entry;

            entry.type = DropType::Item;
            entry.itemId = entryJson.value("item_id", 0);
            entry.weight = entryJson.value("weight",0);
            entry.minCount = entryJson.value("min_count",0);
            entry.maxCount = entryJson.value("max_count",0);
            entry.tag = entryJson.value("tag", "");
           
            group.entries.push_back(entry);
        }
        m_uniqueGroups[group.groupId] = group;
    }   
    return true;

}


std::vector<DropResult> DropManager::SetDropItem(std::string& commonGroup, std::string& uniqueGroup)
{
   
    std::vector<DropResult> dropItems{};
    std::vector<DropEntry> common_entries{};

    auto common_it = m_commonGroups.find(commonGroup);
    auto unique_it = m_uniqueGroups.find(uniqueGroup);
    if(common_it == m_commonGroups.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] [%s] 공통 드롭 그룹에 대한 정보가 없습니다.",__FUNCTION__, __LINE__,commonGroup.c_str());
        return dropItems;
    }

    if(unique_it == m_uniqueGroups.end())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] [%s] 유일 드롭 그룹에 대한 정보가 없습니다.",__FUNCTION__, __LINE__,uniqueGroup.c_str());
        return dropItems;
    }
    
    if(common_it->second.minDrop > common_it->second.maxDrop)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] minDrop [%d] 이 maxDrop [%d]보다 큽니다.",__FUNCTION__, __LINE__,common_it->second.minDrop, common_it->second.maxDrop);
        return dropItems;
    }
   
    if(unique_it->second.minDrop > unique_it->second.maxDrop)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] minDrop [%d] 이 maxDrop [%d]보다 큽니다.",__FUNCTION__, __LINE__,unique_it->second.minDrop, unique_it->second.maxDrop);
        return dropItems;
    }

    // 공통 아이템 생성 
    if(!SelectDropItem(common_it, dropItems))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 생성된 드롭 아이템이 없습니다.",__FUNCTION__, __LINE__);
        return dropItems;
    }

    // 유일 아이템 생성 
    if(!SelectDropItem(unique_it, dropItems))
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] 생성된 드롭 아이템이 없습니다.",__FUNCTION__, __LINE__);
        return dropItems;
    }

    return dropItems;
    
}

int DropManager::CalculateWeight(std::vector<DropEntry>& entries)
{
    int sumWeight = 0;

    if(entries.empty()) return 0;

    for(auto &it : entries)
    {
        sumWeight += it.weight;
    }

    return sumWeight;
}

bool DropManager::SelectDropItem(std::unordered_map<std::string, DropGroup>::iterator Groups, std::vector<DropResult>& dropItems)
{
    DropGroup& group = Groups->second;

    if (group.minDrop < 0 || group.maxDrop < 0 || group.minDrop > group.maxDrop)
    {
        K_slog_trace(K_SLOG_ERROR,
            "[%s][%d] invalid drop range. minDrop=%d, maxDrop=%d",
            __FUNCTION__, __LINE__, group.minDrop, group.maxDrop);
        return false;
    }

    if (group.entries.empty())
    {
        K_slog_trace(K_SLOG_ERROR,
            "[%s][%d] drop entries is empty.",
            __FUNCTION__, __LINE__);
        return false;
    }

    std::uniform_int_distribution<int> randDropCount(group.minDrop, group.maxDrop);
    int dropCount = randDropCount(m_gen);

    std::vector<DropEntry> entries = group.entries;

    for (int i = 0; i < dropCount; ++i)
    {
        if (entries.empty())
        {
            break;
        }

        int sumWeight = CalculateWeight(entries);

        if (sumWeight <= 0)
        {
            K_slog_trace(K_SLOG_ERROR,
                "[%s][%d] invalid sumWeight=%d",
                __FUNCTION__, __LINE__, sumWeight);
            break;
        }

        std::uniform_int_distribution<int> randWeight(1, sumWeight);
        int dropWeight = randWeight(m_gen);

        int accWeight = 0;
        bool selected = false;

        for (size_t idx = 0; idx < entries.size(); ++idx)
        {
            if (entries[idx].weight <= 0)
            {
                continue;
            }

            accWeight += entries[idx].weight;

            if (dropWeight <= accWeight)
            {
                const DropEntry& selected_item = entries[idx];

                if (selected_item.minCount < 0 ||
                    selected_item.maxCount < 0 ||
                    selected_item.minCount > selected_item.maxCount)
                {
                    K_slog_trace(K_SLOG_ERROR,
                        "[%s][%d] invalid item count range. itemId=%d, minCount=%d, maxCount=%d",
                        __FUNCTION__, __LINE__,
                        selected_item.itemId,
                        selected_item.minCount,
                        selected_item.maxCount);

                    return false;
                }

                std::uniform_int_distribution<int> randCount(
                    selected_item.minCount,
                    selected_item.maxCount
                );

                DropResult result;
                result.type = selected_item.type;
                K_slog_trace(K_SLOG_DEBUG,"[%s][%d] selected_item.itemId [%d]", __FUNCTION__, __LINE__, selected_item.itemId);
                result.itemId = selected_item.itemId;
                result.count = randCount(m_gen);

                dropItems.push_back(result);

                if (!group.allowDuplicate)
                {
                    entries.erase(entries.begin() + idx);
                }

                selected = true;
                break;
            }
        }

        if (!selected)
        {
            K_slog_trace(K_SLOG_ERROR,
                "[%s][%d] no drop item selected. dropWeight=%d, sumWeight=%d, entries=%zu",
                __FUNCTION__, __LINE__,
                dropWeight, sumWeight, entries.size());

            break;
        }
    }

    return true;
}
