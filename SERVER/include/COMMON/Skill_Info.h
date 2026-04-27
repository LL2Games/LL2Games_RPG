#pragma once
#include "common.h"
#include "PlayerData.h"

enum class SkillType 
{
    NONE,
    BASIC,
    MELEE_ARC,
    ENUMEND
};
enum class HitShape  
{
    NONE,
    ARC, 
    ENUMEND
};

enum class EffectType 
{
    NONE,
    KNOCKBACK, 
    ENUMEND
};

struct EffectDef
{
    EffectType type;
    float value; // KNOCKBACK이면 force, SLOW면 ratio 같은 식
};

struct SkillDef
{
    int skill_id;
    std::string key;

    SkillType type;

    struct Hit
    {
        HitShape shape;
        float range;
        float angle_deg;
        int max_targets;
        int hit_count;
    } hit;

    struct Damage
    {
        float multiplier;
        int flat_add;
    } damage;

    struct Requirements
    {
        RootJob root_job;
        int min_tier;
        int min_skill_level;
        
    } Requirements;

    int cooldown_ms;
    int mp_cost;

    std::vector<EffectDef> effects;
};

struct LearnedSkill
{
    // 나중에 확장을 위해서 구조체로 정의
    int skill_id;
    int skill_level;
};

struct LearnedSkillSlot
{
    int slot_index;
    int skill_id;
};

namespace Skill
{
    inline SkillType SetSkillType(std::string skillType )
    {
        if(skillType == "BASIC") return SkillType::BASIC;
        if(skillType == "MELEE_ARC") return SkillType::MELEE_ARC;
        return SkillType::NONE;
    };

    inline HitShape SetHitShape(std::string hitType)
    {
        if(hitType == "ARC") return HitShape::ARC;
        return HitShape::NONE;
    };

    inline EffectType SetEffectType(std::string effectType)
    {
        if(effectType == "KNOCKBACK") return EffectType::KNOCKBACK;
        return EffectType::NONE;
    };

}
