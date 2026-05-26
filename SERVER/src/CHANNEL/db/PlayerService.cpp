#include "PlayerService.h"
#include "RedisUtility.h"
#include "CharacterStat.h"
#include "InventroyManager.h"
#include "QuickSlotManager.h"


#include "K_slog.h"
#include <cstdio>
#include <sstream>
#include <string>


MySqlConnectionPool *PlayerService::m_mySql = MySqlConnectionPool::GetInstance();
RedisClient *PlayerService::m_redis = nullptr;

PlayerService::PlayerService()
{
    m_redis = RedisClient::GetInstance();
}

PlayerService::~PlayerService()
{
}

std::unique_ptr<Player> PlayerService::LoadPlayer(int characterId)
{
    std::unique_ptr<Player> player = nullptr;
    PlayerInitData playerInit{};
    std::map<std::string, std::string> redis_map;
    int result = 0;

    if (m_redis != nullptr)
    {
        auto redis_value = m_redis->HGetAll("characterID:" + std::to_string(characterId));
        if (redis_value.has_value() && !redis_value->empty())
        {
            player = std::make_unique<Player>();

            GetStr(*redis_value, "name", playerInit.name);
            GetStr(*redis_value, "account_id", playerInit.account_id);
            GetInt(*redis_value, "level", playerInit.level);
            GetInt(*redis_value, "job", playerInit.job);
            GetInt(*redis_value, "root_job", playerInit.root_job);
            GetInt(*redis_value, "map_id", playerInit.map_id);
            GetFloat(*redis_value, "xPos", playerInit.xPos);
            GetFloat(*redis_value, "yPos", playerInit.yPos);

            BaseStat base;
            DerivedStat derived;
            int curHp;
            int curMp;
            int remainAp;

            GetInt(*redis_value, "base_str", base.str);
            GetInt(*redis_value, "base_dex", base.dex);
            GetInt(*redis_value, "base_intel", base.intel);
            GetInt(*redis_value, "base_luck", base.luck);
            GetInt(*redis_value, "derived_maxHp", derived.maxHp);
            GetInt(*redis_value, "derived_maxMp", derived.maxMp);
            GetInt(*redis_value, "curHp", curHp);
            GetInt(*redis_value, "curMp", curMp);
            GetInt(*redis_value, "remainAp", remainAp);

            CharacterStat stat(base, derived, curHp, curMp, remainAp);
            player->SetInitData(playerInit, stat);

            K_slog_trace(K_SLOG_TRACE, "[Redis]LoadPlayer_stat SUCCESS [%d]", player->GetId());
            return player;
        }
    }

    MYSQL_RES* res = nullptr;
    MYSQL_ROW row;
    MYSQL* conn = m_mySql->GetConnection();

    if (!conn)
    {
        return nullptr;
    }

    std::string query ="SELECT * FROM `character` WHERE char_id = " + std::to_string(characterId);

    result = mysql_query(conn, query.c_str());
    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());

        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    res = mysql_store_result(conn);
    if (!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer ERROR [%s]", mysql_error(conn));

        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    if ((row = mysql_fetch_row(res)) != nullptr)
    {
        K_slog_trace(K_SLOG_TRACE, "LoadPlayer SUCCESS");

        player = std::make_unique<Player>();

        playerInit.char_id = std::atoi(row[0]);
        playerInit.account_id = row[1];
        playerInit.name = row[2];
        playerInit.level = std::atoi(row[3]);
        playerInit.job = std::atoi(row[4]);
        playerInit.root_job = std::atoi(row[7]);

        K_slog_trace(K_SLOG_TRACE, "LoadPlayer SUCCESS [%d]", player->GetId());
    }
    else
    {
        mysql_free_result(res);
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    mysql_free_result(res);
    res = nullptr;

    K_slog_trace(K_SLOG_TRACE, "PlayerInfoToRedisMap Start");

    query =
        "SELECT * FROM character_stat WHERE char_id = " + std::to_string(characterId);

    result = mysql_query(conn, query.c_str());
    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer_stat ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());

        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    res = mysql_store_result(conn);
    if (!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer_stat ERROR [%s]", mysql_error(conn));

        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    if ((row = mysql_fetch_row(res)) != nullptr)
    {
        K_slog_trace(K_SLOG_TRACE, "LoadPlayer_stat SUCCESS");

        BaseStat base;
        DerivedStat derived;
        int curHp;
        int curMp;
        int remainAp;

        base.str = std::atoi(row[1]);
        base.dex = std::atoi(row[2]);
        base.intel = std::atoi(row[3]);
        base.luck = std::atoi(row[4]);

        derived.maxHp = std::atoi(row[5]);
        derived.maxMp = std::atoi(row[6]);

        curHp = std::atoi(row[7]);
        curMp = std::atoi(row[8]);
        remainAp = std::atoi(row[9]);

        CharacterStat stat(base, derived, curHp, curMp, remainAp);

        player->SetInitData(playerInit, stat);
        redis_map = PlayerInfoToRedisMap(playerInit, stat);

        K_slog_trace(K_SLOG_TRACE, "[MySQL]LoadPlayer_stat SUCCESS [%d]", player->GetId());
    }
    else
    {
        mysql_free_result(res);
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    K_slog_trace(K_SLOG_TRACE, "PlayerInfoToRedisMap SUCCESS");

    result = m_redis->HSetAll("characterID", redis_map, 600);

    K_slog_trace(K_SLOG_TRACE, "HSetAll SUCCESS");

    if (result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "HSetAll ERROR");

        mysql_free_result(res);
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    mysql_free_result(res);
    m_mySql->ReleaseConnection(conn);

    return player;
}

bool PlayerService::LoadInventoryMeta(Player* player)
{
    int result = 0;
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL* conn = m_mySql->GetConnection(); 

    auto InventoryManager = player->GetInventoryManager();
    if(!conn) return false;
    std::string query = "SELECT inventory_type, max_slot, current_slot_count FROM character_inventory_meta WHERE char_id = " + std::to_string(player->GetId());
    result = mysql_query(conn, query.c_str());
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadInventoryMeta ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_mySql->ReleaseConnection(conn);
        return false;
    }

    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadInventoryMeta ERROR [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return false;
    }

    while((row = mysql_fetch_row(res)) != nullptr)
    {
        InventoryMetaInfo inventoryMetaInfo;
        inventoryMetaInfo.inventoryType = std::atoi(row[0]);
        inventoryMetaInfo.max_slots = std::atoi(row[1]);
        inventoryMetaInfo.currnet_slots_size = std::atoi(row[2]);
        
        InventoryManager->CreateInventory(inventoryMetaInfo);
    }

    K_slog_trace(K_SLOG_TRACE, "LoadInventoryMeta Success");

    mysql_free_result(res);
    m_mySql->ReleaseConnection(conn);
    return true;

}


bool PlayerService::LoadInventory(Player* player)
{
    int result = 0;
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL* conn = m_mySql->GetConnection(); 
    if(!conn) return false;

    
    std::string query = "SELECT inventory_type, slot_pos, item_Id, item_count FROM character_inventory WHERE char_id = " + std::to_string(player->GetId());
    result = mysql_query(conn, query.c_str());
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadInventory ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_mySql->ReleaseConnection(conn);
        return false;
    }

    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadInventory ERROR [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return false;
    }

    auto inventoryManager = player->GetInventoryManager();

    // 캐릭터 인벤토리 아이템 저장
    while((row = mysql_fetch_row(res)) != nullptr)
    {
        int inventoryType = std::atoi(row[0]);
        int slotpos = std::atoi(row[1]);
        int itemId = std::atoi(row[2]);
        int itemCount = std::atoi(row[3]);
        
        auto inventory =inventoryManager->GetInventory(inventoryType);
        if(inventory == nullptr)
        {
            K_slog_trace(K_SLOG_ERROR, "inventory is nullptr");
            break;
        }
        inventory->SetSlotItem(slotpos, itemId, itemCount);
    }

    mysql_free_result(res);
    m_mySql->ReleaseConnection(conn);
    return true;
}

bool PlayerService::LoadLearnedSkill(Player* player)
{
    int result = 0;
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL* conn = m_mySql->GetConnection(); 
    if(!conn) return false;

    
    std::string query = "SELECT skill_id, skill_level FROM character_skill WHERE char_id = " + std::to_string(player->GetId());
    result = mysql_query(conn, query.c_str());
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadSkill ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_mySql->ReleaseConnection(conn);
        return false;
    }

    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadSkill ERROR [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return false;
    }


    // 캐릭터 인벤토리 아이템 저장
    while((row = mysql_fetch_row(res)) != nullptr)
    {
        LearnedSkill learnedSkill;
        learnedSkill.skill_id = std::atoi(row[0]);
        learnedSkill.skill_level = std::atoi(row[1]);

        player->SetLearnedSkill(learnedSkill);
    }

    mysql_free_result(res);
    m_mySql->ReleaseConnection(conn);
    return true;
}

bool PlayerService::LoadSlotSetting(Player* player)
{
    int result = 0;
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL* conn = m_mySql->GetConnection(); 
    if(!conn) return false;

    /*
     int slot_index;
    QuickSlotType type;
    int ref_id;   // skill_id 또는 item_id 등
    int inventory_type;
    int inventory_slotPos;
    */
    std::string query = "SELECT slot_index, slot_type, ref_id, inventory_type, inventory_slotPos FROM character_skill_slot WHERE char_id = " + std::to_string(player->GetId());
    result = mysql_query(conn, query.c_str());
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadSlotSetting ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_mySql->ReleaseConnection(conn);
        return false;
    }

    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadSlotSetting ERROR [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return false;
    }

    auto quickSlotManager = player->GetQuickSlotManager();

    if(!quickSlotManager)
    {
        K_slog_trace(K_SLOG_ERROR, "quickSlotManager is nullptr [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return false;
    }
    // 퀵슬롯 정보 저장
    while((row = mysql_fetch_row(res)) != nullptr)
    {
        QuickSlotData quickSlot;
        quickSlot.slot_index = std::atoi(row[0]);
        quickSlot.type = QuickSlot::SetSlotType(std::atoi(row[1]));
        quickSlot.ref_id = std::atoi(row[2]);
        quickSlot.inventory_type = std::stoi(row[3]);
        quickSlot.inventory_slotPos = std::stoi(row[4]);
        quickSlotManager->SetSlot(quickSlot);
    }

    mysql_free_result(res);
    m_mySql->ReleaseConnection(conn);
    return true;
}