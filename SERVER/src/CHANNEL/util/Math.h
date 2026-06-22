#pragma once
#include <cmath>

struct Vec2{
    float xPos;
    float yPos;
};

static inline float Dot(const Vec2& a, const Vec2& b) { return a.xPos*b.xPos + a.yPos*b.yPos; }
static inline Vec2  Sub(const Vec2& a, const Vec2& b) { return {a.xPos-b.xPos, a.yPos-b.yPos}; }

static inline float Distance(const Vec2& a, const Vec2& b)
{
    const float dx = a.xPos - b.xPos;
    const float dy = a.yPos - b.yPos;
    return std::sqrt(dx * dx + dy * dy);
};

// 거리 비교할 때 쓴다.
static inline float Distancesquare(const Vec2& a, const Vec2& b)
{
    const float dx = a.xPos - b.xPos;
    const float dy = a.yPos - b.yPos;
    return dx * dx + dy * dy;
}

bool IsHitFrontBox2D(
    const Vec2& attackerPos,
    int facingSign,            // -1: Left, +1: Right (공격자 기준)
    const Vec2& targetPos,
    float range,               // 예: 120
    float verticalTolerance    // 예: 60 (스킬별 튜닝)
);




float Length(const Vec2& v);