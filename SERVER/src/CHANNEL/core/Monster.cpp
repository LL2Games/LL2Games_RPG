#include "Monster.h"


int Monster::Init(const MonsterTemplate& monsterTemplate, const MonsterSpawnData& monsterspawnData)
{
	
	m_name = monsterTemplate.name;
	m_hp = monsterTemplate.hp;
	m_maxhp = monsterTemplate.hp;
	m_exp	= monsterTemplate.exp;
	m_attackDamage = monsterTemplate.attackDamage;
	m_level = monsterTemplate.level;
	m_moveSpeed = monsterTemplate.moveSpeed;
	m_isAlive = true;
	
	m_Pos.xPos = monsterspawnData.spawnPos.xPos;
	m_Pos.yPos = monsterspawnData.spawnPos.yPos;
	
	m_spawnPos.xPos = monsterspawnData.spawnPos.xPos;
	m_spawnPos.yPos = monsterspawnData.spawnPos.yPos;
	
	m_respawnDelay = std::chrono::seconds(monsterspawnData.respawnDelay);
	
	return 1;	
}



bool Monster::CheckRespawnTime(std::chrono::steady_clock::time_point now)
{
	if(now - m_deadTime >= m_respawnDelay)
		return true;
	
	return false;
}


int Monster::Reset()
{
	m_Pos.xPos = m_spawnPos.xPos;
	m_Pos.yPos = m_spawnPos.yPos;
	m_hp = m_maxhp;
	m_isAlive = true;
	return 1;
}