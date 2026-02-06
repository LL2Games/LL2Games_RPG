#pragma once
#include "common.h"

enum class SkillType {MELEE_ARC};
enum class HitShape  {ARC };

enum class EffectType {KNOCKBACK };

struct EffectDef
{
    EffectType type;
    float value; // KNOCKBACK이면 force, SLOW면 ratio 같은 식
};

struct SkillDef
{
    std::string skill_id;
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
        std::string root;
        int min_tier;
        int min_skill_level;
        
    } Requirements;

    int cooldown_ms;
    int mp_cost;

    std::vector<EffectDef> effects;
};
