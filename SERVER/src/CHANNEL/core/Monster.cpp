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
	m_itemGroup = monsterspawnData.ItemId;
	m_instanceId = monsterspawnData.instanceId;
	return 1;	
}

int Monster::Update()
{
	return 0;
}

int Monster::Dead()
{
	if(m_isAlive && !m_deadRequest)
	{
		m_deadRequest = true;
		m_deadTime = std::chrono::steady_clock::now();
		m_isAlive = false;	
	}
	
	return 0;
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


void Monster::OnDamaged(int attackerId, int damage)
{
	// 죽은 뒤 / 죽는 중이라면 무시
	if(!m_isAlive || m_deadRequest) return;

	m_lastAttacker = attackerId;

	m_hp -= damage;

	if(m_hp <= 0)
		Dead();
}