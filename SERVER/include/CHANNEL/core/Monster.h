#pragma once
#include "CHANNEL/core/common.h"
#include "COMMON/CommonEnum.h"

class Monster
{
public:
    Monster(){};
    ~Monster(){};
	
	// 몬스터 정보와 스폰 정보 설정 
	int Init(const MonsterTemplate& monsterTemplate, const MonsterSpawnData& monsterspawnData);
	
	// 몬스터가 죽고 리스폰 됐을 때 사용
	int Reset();
	
	// 죽고난 후 시간 확인 
	bool CheckRespawnTime(std::chrono::steady_clock::time_point now);
	
	// 몬스터 위치 설정
    void SetPos(Vec2 Pos){this->m_Pos.xPos = Pos.xPos; this->m_Pos.yPos = Pos.yPos;}
	
	bool IsAlive() {return m_isAlive;}
	

private:
    MonsterType m_type;
    Vec2 m_Pos;
	Vec2 m_spawnPos;

    bool m_isAlive;
	
	std::string m_name;
    int m_hp;
    int m_maxhp;

    //몬스터 상태 값 나중에 추가해야함
    //MonsterState state;
	float m_exp;
    float m_attackDamage;
    int m_level;
	int m_moveSpeed;
	
	// 죽었을 때 찍히는 시간
	std::chrono::steady_clock::time_point m_deadTime;
	// 리스폰 시간
	std::chrono::seconds m_respawnDelay;
	

};