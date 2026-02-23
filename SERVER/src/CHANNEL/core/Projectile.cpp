#include "Projectile.h"

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
{}

void Projectile::Update(float dt)
{
    Vec2 delta;
    delta.xPos = m_dir.xPos * m_speed * dt;
    delta.yPos = m_dir.yPos * m_speed * dt;
    m_pos.xPos += delta.xPos;
    m_pos.yPos += delta.yPos;
    m_travelled += Length(delta);
}

bool Projectile::IsExpired() const
{
    return m_travelled >= m_range;
}

Vec2 Projectile::GetPos() const
{
    return m_pos;
}