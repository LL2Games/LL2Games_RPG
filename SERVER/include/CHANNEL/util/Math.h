#include "CommonEnum.h"

static inline float Dot(const Vec2& a, const Vec2& b) { return a.xPos*b.xPos + a.yPos*b.yPos; }
static inline Vec2  Sub(const Vec2& a, const Vec2& b) { return {a.xPos-b.xPos, a.yPos-b.yPos}; }

bool IsHitFrontBox2D(
    const Vec2& attackerPos,
    int facingSign,            // -1: Left, +1: Right (공격자 기준)
    const Vec2& targetPos,
    float range,               // 예: 120
    float verticalTolerance    // 예: 60 (스킬별 튜닝)
);
