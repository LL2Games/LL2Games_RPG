#include "MonsterManager.h"
#include <fstream>


#define MONSTER_PATH "./data/Monsters/"


MonsterManager *MonsterManager::m_instance =nullptr;

MonsterManager::MonsterManager()
{
	
}

MonsterManager *MonsterManager::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new MonsterManager();
        //if(m_instance->Init() != EXIT_SUCCESS)
        //{
        //    delete m_instance;
        //    m_instance = nullptr;
        //}
    }
    return m_instance;
}

bool MonsterManager::EnsureLoaded(int monster_id)
{
    {
        // json 파일에 동시에 접근 못하게 락
        std::lock_guard<std::mutex> lock(m_mtx);
        if(m_mops.find(monster_id) != m_mops.end())
        {
            return true;
        }
    }
  
    MonsterTemplate monsterTemplate;

    // Json 파일에서 몬스터 정보 로드
    if(!LoadFromFile(monster_id, monsterTemplate))
    {
        return false;
    }

    // 데이터 삽입 ( 다시 락 )
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        // 짧은 사이에 다른 스레드가 먼저 삽입했을 수 있으니 다시 한번 체크
        /*  move를 사용하는 이유 
            move를 사용안하면 moveTemplate의 모든 값을 복사하는 과정을 거쳐 (원본과 복사본 2개가 생김) 
            따라서 move를 사용해서 복사과정없이 바로 데이터 자체를 옮겨서 성능 최적화
        */
        if(m_mops.find(monster_id) == m_mops.end()) {
            m_mops.emplace(monster_id, std::move(monsterTemplate));
        }
       
    }
    return true;
}

std::optional<MonsterTemplate> MonsterManager::GetMonsterData(int monster_id)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    auto it = m_mops.find(monster_id);
    if(it != m_mops.end())
    {
        return it->second;
    }
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] MonsterData 없음", __FUNCTION__, __LINE__);
    return std::nullopt;
}


bool MonsterManager::LoadFromFile(int monster_id, MonsterTemplate& monsterTemplate)
{

    std::string path= MONSTER_PATH + std::to_string(monster_id) +".json";
    std::ifstream file(path);

    if(!file.is_open()) {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return false;
    }

    // JSON 파일 파싱
    nlohmann::json j;
    file >> j;
	
	/*
	struct MonsterTemplate {
    int monsterId;
    int level;
    int hp;
    int attackDamage;
    float exp;
    float moveSpeed;
    std::string name;
};
	
	*/
	monsterTemplate.monsterId = j["monster_id"];
	monsterTemplate.name = j["name"];
	monsterTemplate.level = j["level"];
	monsterTemplate.hp	= j["hp"];
	monsterTemplate.attackDamage = j["attackDamage"];
	monsterTemplate.exp = j["exp"];
	monsterTemplate.moveSpeed = j["moveSpeed"];
	
	return true;
}


