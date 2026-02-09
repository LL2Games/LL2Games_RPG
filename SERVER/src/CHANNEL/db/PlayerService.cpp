#include "CHANNEL/db/PlayerService.h"
#include "CHANNEL/db/RedisUtility.h"
#include "K_slog.h"
#include <cstdio>
#include <sstream>
#include <string>
#include "CharacterStat.h"

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
    if(m_redis != nullptr)
    {
        auto redis_value = m_redis->HGetAll("characterID:" + std::to_string(characterId));
        if(redis_value.has_value() && !redis_value->empty())
        {
            //redis에 저장된 플레이어의 정보를 player객체에 설정한다.
           
            player = std::make_unique<Player>();
            
            GetStr(*redis_value,"name",playerInit.name);
            GetStr(*redis_value,"account_id",playerInit.account_id);
            GetInt(*redis_value,"level",playerInit.level);
            GetInt(*redis_value,"job",playerInit.job);
            GetInt(*redis_value,"map_id",playerInit.map_id);
            GetFloat(*redis_value,"xPos",playerInit.xPos);
            GetFloat(*redis_value,"yPos",playerInit.yPos);
            //stat 가져오는 로직 추가
            BaseStat base;
            DerivedStat derived;
            int curHp;
            int curMp;
            int remainAp;

            GetInt(*redis_value,"base_str",base.str);
            GetInt(*redis_value,"base_dex",base.dex);
            GetInt(*redis_value,"base_intel",base.intel);
            GetInt(*redis_value,"base_luck",base.luck);
            GetInt(*redis_value,"derived_maxHp",derived.maxHp);
            GetInt(*redis_value,"derived_maxMp",derived.maxMp);
            GetInt(*redis_value,"curHp",curHp);
            GetInt(*redis_value,"curMp",curMp);
            GetInt(*redis_value,"remainAp",remainAp);

            CharacterStat stat(base, derived, curHp, curMp, remainAp);
            player->SetInitData(playerInit, stat);
            K_slog_trace(K_SLOG_TRACE, "[Redis]LoadPlayer_stat SUCCESS [%d]", player->GetId());    
            return player;
        }
    }

    // redis에 저장된 값이 없는 경우 mySQL에서 조회해서 가지고 온다.
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL* conn = m_mySql->GetConnection(); 
    if(!conn) return nullptr;
    std::string query = "SELECT * FROM `character` WHERE char_id = " + std::to_string(characterId);
    result = mysql_query(conn, query.c_str());
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }
    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer ERROR [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    if((row = mysql_fetch_row(res)) != nullptr)
    {
        K_slog_trace(K_SLOG_TRACE, "LoadPlayer SUCCESS");    
        player = std::make_unique<Player>();

        playerInit.char_id = std::atoi(row[0]);
        
        playerInit.account_id = row[1];
        playerInit.name = row[2];
        playerInit.level = std::atoi(row[3]);
        playerInit.job = std::atoi(row[4]);

        K_slog_trace(K_SLOG_TRACE, "LoadPlayer SUCCESS [%d]", player->GetId());    
    }
     K_slog_trace(K_SLOG_TRACE, "PlayerInfoToRedisMap Start"); 

    //stat
    query = "SELECT * FROM character_stat WHERE char_id = " + std::to_string(characterId);
    result = mysql_query(conn, query.c_str());
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer_stat ERROR [%s]", mysql_error(conn));
        K_slog_trace(K_SLOG_ERROR, "SQL [%s]", query.c_str());
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }
    res = mysql_store_result(conn);
    if(!res)
    {
        K_slog_trace(K_SLOG_ERROR, "LoadPlayer_stat ERROR [%s]", mysql_error(conn));
        m_mySql->ReleaseConnection(conn);
        return nullptr;
    }

    if((row = mysql_fetch_row(res)) != nullptr)
    {
        K_slog_trace(K_SLOG_TRACE, "LoadPlayer_stat SUCCESS");    
        BaseStat base;
        DerivedStat derived;
        int curHp;
        int curMp;
        int remainAp;

        //row[0] == char_id
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
        
        player->SetInitData(playerInit,stat);
        redis_map = PlayerInfoToRedisMap(playerInit, stat);
        K_slog_trace(K_SLOG_TRACE, "[MySQL]LoadPlayer_stat SUCCESS [%d]", player->GetId());    
    }


    K_slog_trace(K_SLOG_TRACE, "PlayerInfoToRedisMap SUCCESS"); 

    result =  m_redis->HSetAll("characterID", redis_map, 600);

    K_slog_trace(K_SLOG_TRACE, "HSetAll SUCCESS"); 
    
    if(result != 0)
    {
        K_slog_trace(K_SLOG_ERROR, "HSetAll ERROR"); 
        return nullptr;
    }

    mysql_free_result(res);
    m_mySql->ReleaseConnection(conn);
    return player;
}
