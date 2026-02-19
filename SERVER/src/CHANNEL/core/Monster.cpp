#include "Monster.h"
#include "Player.h"
#include <math.h>
#include "MapInstance.h"
#include "K_slog.h"
#include "timeUtility.h"

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

	//스폰된 맵 ID 초기화
	m_mapId = monsterTemplate.mapId;

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

	//막타 플레이어
	m_lastAttacker = nullptr;
	m_lastAttackerId = 0;

	//원거리공격
	m_isRangedAttack = 0; //예시값, 필요에 따라 조정 monsterspawnData.isRangedAttack로
	m_ragedAttackRange = 50.0f; //예시값, 필요에 따라 조정 monsterspawnData.attackRange로 
	m_attackCooldown = 2000.0f; //예시값, 필요에 따라 조정 monsterspawnData.attackCooldown로 
	m_lastAttackTime = 0.0f;

	return 1;
}

int Monster::Update(float dt)
{
	switch (m_state)
	{
	case E_Patrol:
		K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] state=[Patrol]", __FILE__, __FUNCTION__, __LINE__);
		UpdatePatrol(dt);
		break;
		case E_Chase:
		K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] state=[Chase]", __FILE__, __FUNCTION__, __LINE__);
		UpdateChase(dt);
		break;
		
		case E_Dead:
		K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] state=[Dead]", __FILE__, __FUNCTION__, __LINE__);
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

bool Monster::IsAttackOnCooldown()
{
	auto now = NowMs();
	return m_lastAttackTime == 0 || (now - m_lastAttackTime) >= m_attackCooldown;
}

int Monster::TryRangedAttack(const float& ) //dir
{
	//쿨다운 체크
	if (IsAttackOnCooldown())
	{
		return 0;
	}

	//스킬 사용 로직
	/*
	1. 투사체 생성 및 원거리공격 수행
	2. 원거리공격 state값을 ing로 변경
	3. state값이 ing일경우 service에서 CalculateRangedAttackPos 등을 통해 위치 계산
	4. 계산된 위치로 다음 멤버변수 업데이트
	   m_projectilePos.xPos = 계산된 x좌표;
	   m_projectilePos.yPos = 계산된 y좌표;
	   m_projectileDir = dir;
	4-2 CalculateRagedAttackPos등에서 공격충돌 및 유효거리 도달시 공격 종료
	*/
	

	return 1;
}

int Monster::UpdateChase(float dt)
{
	//m_lastAttackerId가 막타 맞은 플레이어이므로 해당 플레이어 chase모드
	Player *player = m_lastAttacker;

	if (!player) //막타플레이어 없을경우 예외처리
		return 0;

	Vec2 playerPos = player->GetPosition();

	//플레이어가 몬스터와 같은 맵에 없는경우 예외처리
	if (player->GetCurrentMap() && (player->GetCurrentMap()->GetMapId() != m_mapId))
		return UpdatePatrol(dt);

	//플레이어와 몬스터 거리 계산 (X축만 사용)
	float dx = playerPos.xPos - m_Pos.xPos;

	//방향 결정
	if (dx > 0)
		m_dir = 1.0f; //오른쪽
	else
		m_dir = -1.0f; //왼쪽

	//공격 범위 내에 플레이어가 있다면 공격
	if (m_isRangedAttack && fabs(dx) <= m_ragedAttackRange)
	{
		if (TryRangedAttack(m_dir))
		{
			//공격 성공시에만 마지막 공격시간 업데이트
			m_lastAttackTime = NowMs();
		}
		return 0;
	}

	//너무 가까우면 이동하지 않음
	if (fabs(dx) < 5.0f)
		return 0;

	//이동
	m_Pos.xPos += m_dir * m_moveSpeed * dt;
	
	return 0;
}

int Monster::Dead()
{
	if (m_isAlive && !m_deadRequest)
	{
		m_deadRequest = true;
		m_deadTime = std::chrono::steady_clock::now();
		m_isAlive = false;
		m_state = E_Dead;
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
	m_state = E_Patrol;
	return 1;
}

bool Monster::OnDamaged(Player *Attacker, int damage)
{
	// 죽은 뒤 / 죽는 중이라면 무시
	if (!m_isAlive || m_deadRequest)
		return false;
	if (damage <= 0)
		return false;

	m_lastAttackerId = Attacker->GetId();
	m_lastAttacker = Attacker;
	//한대 맞으면 해당 chase 모드로 전환
	m_state = E_Chase;

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		Dead();
		return true;
	}

	return false;
}