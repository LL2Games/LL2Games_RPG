#include "Projectile.h"
#include "K_slog.h"

Projectile::Projectile(const Vec2& startPos,
                const Vec2& dir,
                float speed,
                float range,
                int ownerId) 
            : m_pos(startPos),
              m_dir(dir),
              m_speed(speed),
              m_range(range),
              m_ownerMonsterId(ownerId)
{
}

void Projectile::Update(float dt)
{
    Vec2 delta;
    delta.xPos = m_dir.xPos * m_speed * dt;
    delta.yPos = m_dir.yPos * m_speed * dt;
    m_pos.xPos += delta.xPos;
    m_pos.yPos += delta.yPos;


    K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] Projectile Update dt[%.2f] pos[%.2f, %.2f] delta[%.2f, %.2f]", __FILE__, __FUNCTION__, __LINE__, dt, m_pos.xPos, m_pos.yPos, delta.xPos, delta.yPos);
    m_travelled += Length(delta);
}

bool Projectile::IsExpired() const
{
    K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] Projectile IsExpired travelled[%.2f] range[%.2f]", __FILE__, __FUNCTION__, __LINE__, m_travelled, m_range);
    return m_travelled >= m_range;
}

Vec2 Projectile::GetPos() const
{
    return m_pos;
}