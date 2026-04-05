#include "Math.h"
#include <cmath>


bool IsHitFrontBox2D(
    const Vec2& attackerPos,
    int facingSign,            // -1: Left, +1: Right (공격자 기준)
    const Vec2& targetPos,
    float range,               // 예: 120
    float verticalTolerance    // 예: 60 (스킬별 튜닝)
)
{
    // 1) 세로(플랫폼 감각)
    float dy = targetPos.yPos - attackerPos.yPos;
    if (std::fabs(dy) > verticalTolerance)
        return false;

    // 2) 가로(공격자 "앞쪽"만)
    float dx = targetPos.xPos - attackerPos.xPos;
    float front = dx * (float)facingSign; // 왼쪽이면 -dx, 오른쪽이면 +dx

    // 앞쪽이 아니면(front < 0) 미스
    if (front < 0.0f)
        return false;

    // 사거리 밖이면 미스
    if (front > range)
        return false;

    return true;
}