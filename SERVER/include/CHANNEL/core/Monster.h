#pragma once
#include "common.h"
#include "CommonEnum.h"
//#include "Player.h"

class Player;

struct MonsterHitResult {
    int monster_instance_id;
    int damage;
    int cur_hp;
    int max_hp;
    bool dead;
};



class Monster
{
public:
    Monster(){};
    ~Monster(){};
	
	// 몬스터 정보와 스폰 정보 설정 
	int Init(const MonsterTemplate& monsterTemplate, const MonsterSpawnData& monsterspawnData);
	
	// 몬스터 업데이트
	int Update();

	// 몬스터 사망
	int Dead();

	// 몬스터가 죽고 리스폰 됐을 때 사용
	int Reset();

	bool OnDamaged(Player* Attacker, int damage);
	
	// 죽고난 후 시간 확인 
	bool CheckRespawnTime(std::chrono::steady_clock::time_point now);
	
	
	
public:
	// 몬스터 위치 설정
    void SetPos(Vec2 Pos){this->m_Pos.xPos = Pos.xPos; this->m_Pos.yPos = Pos.yPos;}
	Vec2 GetPos(){return m_Pos;}

	int GetLevel() const {return m_level;}

	bool IsAlive() {return m_isAlive;}

	int GetLastAttackerID() {return m_lastAttacker;}

	float GetExp(){return m_exp;}

	int GetItemGroup(){return m_itemGroup;}

	int GetInstanceId() const {return m_instanceId;}

	int GetCurrentHP() {return m_hp;}
	int GetMaxHP() {return m_maxhp;}
private:
    MonsterType m_type;
    Vec2 m_Pos;
	Vec2 m_spawnPos;

    bool m_isAlive;
	bool m_deadRequest; 
	
	std::string m_name;
    int m_hp;
    int m_maxhp;

    //몬스터 상태 값 나중에 추가해야함
    //MonsterState state;
	float m_exp;
    float m_attackDamage;
    int m_level;
	int m_moveSpeed;

	// 같은 몬스터라도 구별할 수 있는 고유 값이 필요하기 때문에 추가
	int m_instanceId;
	
	// 죽었을 때 찍히는 시간
	std::chrono::steady_clock::time_point m_deadTime;
	// 리스폰 시간
	std::chrono::seconds m_respawnDelay;

	// 몬스터 아이템 드롭 그룹
	int m_itemGroup;
	
	int m_lastAttacker;

};