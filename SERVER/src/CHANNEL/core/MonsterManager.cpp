#include "MonsterManager.h"
#include <mutex>




bool MonsterManager::EnsureLoaded(int monster_id)
{
    // 동시에 json 파일에 ㅈ버
    std::lock_guard<std::mutex> lock(m_mtx);

    if(m_mops.find(monster_id) != m_mops.end())
    {
        return true;
    }

    MonsterTemplate monsterTemplate;


    
  


    return false;
}

const MonsterTemplate MonsterManager::GetMonsterData(int monster_id)
{

}


bool MonsterManager::LoadFromFile(int monster_id)
{
    
}


