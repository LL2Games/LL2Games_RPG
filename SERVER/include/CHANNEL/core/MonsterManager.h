#include "common.h"
#include "CommonEnum.h"
#include <optional>
#include <nlohmann/json.hpp>
#include <mutex>
class MonsterManager 
{
public:
	
	
	MonsterManager();
	~MonsterManager();
	
	static MonsterManager *GetInstance();

    void Init();
    // 몬스터 정보가 없으면 로드, 있으면 True로 반환
    bool EnsureLoaded(int monster_id);

    // 몬스터 정보가 반드시 존재한다는 가정하에 사용하는 함수 
    // 없으면 nullptr 반환
    std::optional<MonsterTemplate> GetMonsterData(int monster_id);
	
	void RemoveMap();

private: 
    // json 파일에서 몬스터 정보 읽어오는 함수
    bool LoadFromFile(int monster_id, MonsterTemplate& monsterTemplate);
private:
    // 한번 Json파일에서 읽어온 몬스터 정보를 저장하는 맵
    std::unordered_map<int , MonsterTemplate> m_mops;

    std::mutex m_mtx;

	static MonsterManager *m_instance;
};