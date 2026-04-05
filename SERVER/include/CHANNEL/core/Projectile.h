#pragma once
#include "IProjectile.h"
#include "Math.h"

class Projectile : public IProjectile
{
public:
    Projectile(const Vec2& startPos,
                const Vec2& dir,
                float speed,
                float range,
                int ownerId);

    void Update(float dt) override;
    bool IsExpired() const override;
    Vec2 GetPos() const override;
    int GetId() const override { return m_projectileId; }
    Vec2 GetDir() const override { return m_dir; }
    Collider2D GetCollider() const override { return m_collider; }
    virtual int GetDamage() const override { return m_damage; }
protected:
    Vec2 m_pos;
    Vec2 m_dir;
    float m_speed;
    float m_range;
    Collider2D m_collider;
    static constexpr int m_damage = 10; // 예시 데미지, 필요에 따라 조정
    float m_travelled = 0.f;

    int m_ownerMonsterId;
    const int m_projectileId = 1; // 필요하다면 투사체 고유 ID
};