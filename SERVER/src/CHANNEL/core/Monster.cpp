#include "Monster.h"
#include "Player.h"

int Monster::Init(const MonsterTemplate &monsterTemplate, const MonsterSpawnData &monsterspawnData)
{

	m_name = monsterTemplate.name;
	m_hp = monsterTemplate.hp;
	m_maxhp = monsterTemplate.hp;
	m_exp = monsterTemplate.exp;
	m_attackDamage = monsterTemplate.attackDamage;
	m_level = monsterTemplate.level;
	m_moveSpeed = monsterTemplate.moveSpeed;
	m_isAlive = true;

	m_Pos.xPos = monsterspawnData.spawnPos.xPos;
	m_Pos.yPos = monsterspawnData.spawnPos.yPos;

	m_dir = 1.0f;
	m_rightBound = m_Pos.xPos + 3.0f;
	m_leftBound = m_Pos.xPos - 3.0f;

	m_spawnPos.xPos = monsterspawnData.spawnPos.xPos;
	m_spawnPos.yPos = monsterspawnData.spawnPos.yPos;

	m_respawnDelay = std::chrono::seconds(monsterspawnData.respawnDelay);
	m_itemGroup = monsterspawnData.ItemId;
	m_instanceId = monsterspawnData.instanceId;

	m_state = E_Patrol;
	return 1;
}

int Monster::Update(float dt)
{
	switch (m_state)
	{
	case E_Patrol:
		UpdatePatrol(dt);
		break;
	case E_Chase:
		UpdateChase(dt);
		break;

	case E_Dead:
		break;
	}
	return 0;
}

int Monster::UpdatePatrol(float dt)
{
	//이동
	m_Pos.xPos += m_dir * m_moveSpeed * dt;

	//오른쪽 경계 도달
	if (m_Pos.xPos >= m_rightBound)
	{
		m_Pos.xPos = m_rightBound;
		m_dir = -1.0f; //방향전환
	}

	//왼쪽 경계 도달
	if (m_Pos.xPos <= m_leftBound)
	{
		m_Pos.xPos = m_leftBound;
		m_dir = 1.0f; // 방향 전환
	}

	return 0;
}

int Monster::UpdateChase(float )
{
	return 0;
}

int Monster::Dead()
{
	if (m_isAlive && !m_deadRequest)
	{
		m_deadRequest = true;
		m_deadTime = std::chrono::steady_clock::now();
		m_isAlive = false;
	}

	return 0;
}

bool Monster::CheckRespawnTime(std::chrono::steady_clock::time_point now)
{
	if (now - m_deadTime >= m_respawnDelay)
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

bool Monster::OnDamaged(Player *Attacker, int damage)
{
	// 죽은 뒤 / 죽는 중이라면 무시
	if (!m_isAlive || m_deadRequest)
		return false;
	if (damage <= 0)
		return false;

	m_lastAttacker = Attacker->GetId();

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		Dead();
		return true;
	}

	return false;
}