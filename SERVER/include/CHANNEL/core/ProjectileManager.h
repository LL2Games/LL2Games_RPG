#pragma once
#include <memory>
#include <vector>

class IProjectile;

class ProjectileManager
{
public:
    void Add(std::unique_ptr<IProjectile> p);
    void Update(float dt = 1.0f);
    const std::vector<std::unique_ptr<IProjectile>>& GetProjectiles() const { return m_projectiles; }

private:
    std::vector<std::unique_ptr<IProjectile>> m_projectiles;
};