#pragma once
#include "Math.h"
#include <cmath>
#include <algorithm>
#include <cstdint>


enum class ColliderType : uint8_t
{
    None,
    Rect2D,
    Circle2D,
    EnumsEnd
};

struct Rect2D
{
    Vec2 offset{};
    float halfW{0.f};
    float halfH{0.f};
};

struct Circle2D
{
    Vec2 offset{};
    float radius{0.f};
};

struct Collider2D
{
    ColliderType type{ColliderType::None};
    Rect2D rect{};
    Circle2D circle{};
};

namespace Collision
{
    // Rect 끼리 충돌 하는 경우
    inline bool IntersectsRect2D(const Vec2& posA, const Rect2D& a,
                                 const Vec2& posB, const Rect2D& b)
    {
        const float ax = posA.xPos + a.offset.xPos;
        const float ay = posA.yPos + a.offset.yPos;
        const float bx = posB.xPos + b.offset.xPos;
        const float by = posB.yPos + b.offset.yPos;

        return (std::fabs(ax - bx) <= (a.halfW + b.halfW)) &&
               (std::fabs(ay - by) <= (a.halfH + b.halfH));
    }

    // Circle 끼리 충돌하는 경우
    inline bool IntersectsCircle2D(const Vec2& posA, const Circle2D& a,
                                   const Vec2& posB, const Circle2D& b)
    {
        const float ax = posA.xPos + a.offset.xPos;
        const float ay = posA.yPos + a.offset.yPos;
        const float bx = posB.xPos + b.offset.xPos;
        const float by = posB.yPos + b.offset.yPos;

        const float dx = ax - bx;
        const float dy = ay - by;
        const float r  = a.radius + b.radius;

        return (dx*dx + dy*dy) <= (r*r);
    }


    
    // Rect와 Circle이 충돌하는 경우 
    inline bool IntersectsRect2DCircle2D(const Vec2& posRect, const Rect2D& rect,
                                         const Vec2& posCircle, const Circle2D& circle)
    {
        const float rx = posRect.xPos + rect.offset.xPos;
        const float ry = posRect.yPos + rect.offset.yPos;

        const float cx = posCircle.xPos + circle.offset.xPos;
        const float cy = posCircle.yPos + circle.offset.yPos;

        const float minX = rx - rect.halfW;
        const float maxX = rx + rect.halfW;
        const float minY = ry - rect.halfH;
        const float maxY = ry + rect.halfH;

        const float closestX = std::clamp(cx, minX, maxX);
        const float closestY = std::clamp(cy, minY, maxY);

        const float dx = cx - closestX;
        const float dy = cy - closestY;

        return (dx*dx + dy*dy) <= (circle.radius * circle.radius);
    }

    // 충돌 객체들의 Collider Type에 따라서 다른 함수 실행
    inline bool Intersects(const Vec2& posA, const Collider2D& a,
                           const Vec2& posB, const Collider2D& b)
    {
         if (a.type == ColliderType::None || b.type == ColliderType::None)
            return false;

        if (a.type == ColliderType::Rect2D && b.type == ColliderType::Rect2D)
            return Collision::IntersectsRect2D(posA, a.rect, posB, b.rect);

        if (a.type == ColliderType::Circle2D && b.type == ColliderType::Circle2D)
            return Collision::IntersectsCircle2D(posA, a.circle, posB, b.circle);

        if (a.type == ColliderType::Rect2D && b.type == ColliderType::Circle2D)
            return Collision::IntersectsRect2DCircle2D(posA, a.rect, posB, b.circle);

        if (a.type == ColliderType::Circle2D && b.type == ColliderType::Rect2D)
            return Collision::IntersectsRect2DCircle2D(posB, b.rect, posA, a.circle);

        return false;
    }

    inline ColliderType SetCollisionType(std::string type)
    {
        if(type == "Rect2D") return ColliderType::Rect2D;
        if(type == "Circle2D") return ColliderType::Circle2D;
        return ColliderType::None;
    }


    // Rect(halfW, halfH)를 거리 컷용 원 반지름으로 근사: 빠름(조금 타이트할 수 있음)
    // 플레이어와 몬스터 사이의 거리가 일정 이상인 경우 피격 판정 여부 계산을 넘기기 위해서 사용
    inline float RectToRadiusFast(float halfW, float halfH)
    {
        return std::max(halfW, halfH);
    }

    // Rect를 완전히 포함하는 외접원 반지름: 안전함(조금 큼)
    inline float RectToRadiusSafe(float halfW, float halfH)
    {
        return std::sqrt(halfW*halfW + halfH*halfH);
    }

    // (플레이어 Rect + 몬스터 Rect) 기반 broad-phase 거리 컷 반지름 계산
    // extra는
    inline float MakeContactRadius_Rect(float pHalfW, float pHalfH,
                                 float mHalfW, float mHalfH,
                                 float extra)
    {
        float Rp = RectToRadiusFast(pHalfW, pHalfH);   // 또는 Safe
        float Rm = RectToRadiusFast(mHalfW, mHalfH);

        return Rp + Rm + extra;
    }


    /*
    정말 타이트: 0~4

    broad-phase로 무난: 8~16

    프레임 이동량/네트워크 보정까지 넉넉: 16~32
    */
    inline float MakeContactRadiusSq_Rect(float pHalfW, float pHalfH,
                                          float mHalfW, float mHalfH,
                                          float extra)
    {
        const float r = MakeContactRadius_Rect(pHalfW, pHalfH, mHalfW, mHalfH, extra);
        return r * r;
    }
}


