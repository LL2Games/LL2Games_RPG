#pragma once
#include "CHANNEL/core/common.h"
#include "COMMON/CommonEnum.h"

class Monster
{
public:
    Monster(){};
    ~Monster(){};

    void SetPos(Vec2 Pos){this->m_Pos.xPos = Pos.xPos; this->m_Pos.yPos = Pos.yPos;}

    
private:
    MonsterType m_type;
    Vec2 m_Pos;
    bool m_isAlive;

    int m_hp;
    int m_maxhp;

    //몬스터 상태 값 나중에 추가해야함
    //MonsterState state;
    float m_attackDamage;
    int m_level;

};