#include "ProjectileManager.h"
#include "IProjectile.h"
#include <algorithm>

void ProjectileManager::Add(std::unique_ptr<IProjectile> p)
{
    m_projectiles.push_back(std::move(p));
}

void ProjectileManager::Update(float dt)
{
    for (auto& p : m_projectiles)
    {
        p->Update(dt);
    }

    //만료된 투사체를 remove_if로 뒤로 보내고 새로운 End 얻기
    auto newEnd = std::remove_if(m_projectiles.begin(), m_projectiles.end(),
        [](const auto& p){ return p->IsExpired();});

    //새로운 End부터 끝까지의 요소들을 삭제
    m_projectiles.erase(newEnd, m_projectiles.end());
}