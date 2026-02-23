#pragma once
#include "CommonEnum.h"

class IProjectile
{
public:
    virtual ~IProjectile() = default;
    
    virtual void Update(float dt) = 0;
    virtual bool IsExpired() const = 0;
    virtual Vec2 GetPos() const = 0;
    virtual Vec2 GetDir() const = 0;
    virtual int GetId() const = 0;
};