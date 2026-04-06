#include "MonsterManager.h"
#include <fstream>


#define MONSTER_PATH "../src/CHANNEL/data/Monsters/"
namespace fs = std::filesystem;

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

bool MonsterManager::Init()
{
    if(!PreLoadAll()) return false;
    return true;
}

bool MonsterManager::PreLoadAll()
{
     for (const auto &entry : fs::directory_iterator(MONSTER_PATH))
    {
        if (!entry.is_regular_file())
            continue;
        if (entry.path().extension() != ".json")
            continue;

        /*
            // entry.path() -> 파일 경로
            // entry.path().filename() -> 파일명
            // entry.path().stem() -> 확장자 제거한 파일명(예: "1001")

        */
        int monster_id = std::stoi(entry.path().stem().string());

        auto it = m_mops.find(monster_id);
        if (it != m_mops.end())
            continue;

        MonsterTemplate monsterTemplate;

        bool is_Load = LoadJsonFile(monster_id, monsterTemplate);
        if (!is_Load)
            return false;

        m_mops.emplace(monster_id, std::move(monsterTemplate));
    }
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] MonsterData PreLoadAll Success", __FUNCTION__, __LINE__);
    return true;
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
    if(!LoadJsonFile(monster_id, monsterTemplate))
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


bool MonsterManager::LoadJsonFile(int monster_id, MonsterTemplate& monsterTemplate)
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
    // 이렇게 json 파일을 읽을 때 가져오는 데이터 타입을 정해주면 안전하다.
                                  //= j["monster_id"];
	monsterTemplate.monsterId     = j.at("monster_id").get<int>();
    monsterTemplate.name          = j.at("name").get<std::string>();
    monsterTemplate.level         = j.at("level").get<int>();
    monsterTemplate.hp            = j.at("hp").get<int>();
    monsterTemplate.attackDamage  = j.at("attackDamage").get<int>();
    monsterTemplate.exp           = j.at("exp").get<float>();
    monsterTemplate.moveSpeed     = j.at("moveSpeed").get<float>();

    monsterTemplate.common_drop_group_id = j.at("common_drop_group_id").get<std::string>();
    monsterTemplate.unique_drop_group_id = j.at("unique_drop_group_id").get<std::string>();

    // collider는 현재 JSON상 1개만 있다고 가정하면:
    const auto& col = j.at("collider").at(0);

    monsterTemplate.collisionType = Collision::SetCollisionType(col.at("type").get<std::string>());

    if(monsterTemplate.collisionType == ColliderType::Rect2D)
    {
        monsterTemplate.offset.xPos = col.at("offset").at("x").get<float>();
        monsterTemplate.offset.yPos = col.at("offset").at("y").get<float>();

        monsterTemplate.half.xPos   = col.at("half").at("w").get<float>();
        monsterTemplate.half.yPos   = col.at("half").at("h").get<float>();
    }
    else if(monsterTemplate.collisionType == ColliderType::Circle2D)
    {
        monsterTemplate.offset.xPos = col.at("offset").at("x").get<float>();
        monsterTemplate.offset.yPos = col.at("offset").at("y").get<float>();

        monsterTemplate.radius   = col.at("radius").get<float>();
    }

	
    float Rp = Collision::RectToRadiusFast(MonsterManager::PLAYER_HALF_W, MonsterManager::PLAYER_HALF_H);
    float Rm = Collision::RectToRadiusFast(monsterTemplate.half.xPos, monsterTemplate.half.yPos); // JSON에서 읽은 값
    float r = Rp + Rm + MonsterManager::EXTRA;


    monsterTemplate.broadCutSq = r * r;

    
	return true;
}


