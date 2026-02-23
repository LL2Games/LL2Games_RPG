#include "Monster.h"
#include "Player.h"
#include <math.h>
#include "MapInstance.h"
#include "K_slog.h"
#include "timeUtility.h"
#include "Projectile.h"
#include "ProjectileManager.h"

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
	m_deadRequest = false;

	//스폰된 맵 ID 초기화
	m_mapId = monsterTemplate.mapId;
	m_mapInstance = monsterTemplate.mapInstance;

	m_Pos.xPos = monsterspawnData.spawnPos.xPos;
	m_Pos.yPos = monsterspawnData.spawnPos.yPos;

	m_dir.xPos = 1.0f;
	m_dir.yPos = 0.0f;
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
	m_isRangedAttack = 1; //예시값, 필요에 따라 조정 monsterspawnData.isRangedAttack로
	m_ragedAttackRange = 50.0f; //예시값, 필요에 따라 조정 monsterspawnData.attackRange로 
	m_attackCooldown = 2000.0f; //예시값, 필요에 따라 조정 monsterspawnData.attackCooldown로 
	m_lastAttackTime = 0;

	return 1;
}

int Monster::Update(float dt)
{

	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] state=[%d]", __FILE__, __FUNCTION__, __LINE__, m_state);
	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] Monster Pointer[%p]", __FILE__, __FUNCTION__, __LINE__, this);

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

//x축만 이동
int Monster::UpdatePatrol(float dt)
{
	//이동
	m_Pos.xPos += m_dir.xPos * m_moveSpeed * dt;

	//오른쪽 경계 도달
	if (m_Pos.xPos >= m_rightBound)
	{
		m_Pos.xPos = m_rightBound;
		m_dir.xPos = -1.0f; //방향전환
	}

	//왼쪽 경계 도달
	if (m_Pos.xPos <= m_leftBound)
	{
		m_Pos.xPos = m_leftBound;
		m_dir.xPos = 1.0f; // 방향 전환
	}

	return 0;
}

bool Monster::IsAttackOnCooldown()
{
	auto now = NowMs();
	return !(m_lastAttackTime == 0 || (now - m_lastAttackTime) >= m_attackCooldown);
}

bool Monster::TryRangedAttack(const Vec2& dir)
{
	//쿨다운 체크
	if (IsAttackOnCooldown())
	{
		K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] 공격 쿨다운 중입니다.", __FILE__, __FUNCTION__, __LINE__);
		return false;
	}

	//투사체 생성
	auto projectile = std::make_unique<Projectile>(
		m_Pos,
		dir,
		m_projectileSpeed,
		m_ragedAttackRange,
		m_instanceId
	);

	//맵 인스턴스의 투사체 매니저에 투사체 추가
	m_mapInstance->GetProjectileManager().Add(std::move(projectile));

	//마지막 공격 시간 업데이트
	m_lastAttackTime = NowMs();
	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] 원거리 공격 시도. 방향: %f", __FILE__, __FUNCTION__, __LINE__, dir);

	return true;
}

int Monster::UpdateChase(float dt)
{
	//m_lastAttackerId가 막타 맞은 플레이어이므로 해당 플레이어 chase모드
	Player *player = m_lastAttacker;

	if (!player) //막타플레이어 없을경우 예외처리
	{
		K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] No attacker to chase.", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	Vec2 playerPos = player->GetPosition();

	//플레이어가 몬스터와 같은 맵에 없는경우 예외처리
	if (player->GetCurrentMap() && (player->GetCurrentMap()->GetMapId() != m_mapId))
	{
		K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] Attacker is on a different map.", __FILE__, __FUNCTION__, __LINE__);
		return UpdatePatrol(dt);
	}

	//플레이어와 몬스터 거리 계산 (X축만 사용)
	float dx = playerPos.xPos - m_Pos.xPos;
	float dy = playerPos.yPos - m_Pos.yPos;

	//방향 결정
	if (dx > 0)
		m_dir.xPos = 1.0f; //오른쪽
	else
		m_dir.xPos = -1.0f; //왼쪽

	if (dy > 0)
		m_dir.yPos = 1.0f; //위
	else
		m_dir.yPos = -1.0f; //아래

	//공격 범위 내에 플레이어가 있다면 공격
	if (m_isRangedAttack && fabs(dx) <= m_ragedAttackRange)
	{
		K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] Player is within ranged attack range. Attempting attack.", __FILE__, __FUNCTION__, __LINE__);
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
	m_Pos.xPos += m_dir.xPos * m_moveSpeed * dt;
	m_Pos.yPos += m_dir.yPos * m_moveSpeed * dt;
	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] Chasing player. New position: (%f, %f)", __FILE__, __FUNCTION__, __LINE__, m_Pos.xPos, m_Pos.yPos);
	
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
	//test
	return true;

	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] isAlive: %d, now - m_deadTime: %d, m_respawnDelay: %d", __FILE__, __FUNCTION__, __LINE__, m_isAlive, now.time_since_epoch().count() - m_deadTime.time_since_epoch().count(), m_respawnDelay.count());

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
	m_deadRequest = false;
	m_state = E_Patrol;
	m_lastAttackTime = 0.0f;
	return 1;
}

bool Monster::OnDamaged(Player *Attacker, int damage)
{
	K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] AttackerId[%d] damage[%d]", __FILE__, __FUNCTION__, __LINE__, Attacker->GetId(), damage);
	K_slog_trace(K_SLOG_TRACE, "[%s:%s][%d] Monster Pointer[%p]", __FILE__, __FUNCTION__, __LINE__, this);

	// 죽은 뒤 / 죽는 중이라면 무시
	if (!m_isAlive || m_deadRequest)
	{
		K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 이미 죽은 몬스터입니다.", __FILE__, __FUNCTION__, __LINE__);
		return false;
	}
	if (damage <= 0)
	{
		K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] damage[%d] <= 0", __FILE__, __FUNCTION__, __LINE__, damage);
		return false;
	}

	m_lastAttackerId = Attacker->GetId();
	m_lastAttacker = Attacker;
	//한대 맞으면 해당 chase 모드로 전환
	m_state = E_Chase;
	K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 몬스터가 플레이어 %d에게 공격당했습니다. 남은 HP: %d", __FILE__, __FUNCTION__, __LINE__, Attacker->GetId(), m_hp - damage);

	K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 몬스터 상태[%d]", __FILE__, __FUNCTION__, __LINE__, m_state);

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		Dead();
		K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 몬스터 죽음", __FILE__, __FUNCTION__, __LINE__);
		return true;
	}

	K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] End", __FILE__, __FUNCTION__, __LINE__);
	return false;
}