#include "CharacterService.h"
#include "K_slog.h"
#include "RedisClient.h"
#include "MySqlConnectionPool.h"

std::vector<std::string> CharacterService::GetCharacterList(const std::string& account_id)
{
    std::vector<std::string> char_list;

    //test
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] id[%s]", __FUNCTION__, __LINE__, account_id);

    //1.Redis charlist 조회
    //2.Redis miss-> MySQL 조회
    //3.응답값 리턴
    return char_list;
}