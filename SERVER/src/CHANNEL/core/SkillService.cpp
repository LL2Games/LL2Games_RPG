#include "SkillService.h"


#define SKILL_PATH "../src/CHANNEL/data/Skill"

SkillService *SkillService::m_instance =nullptr;


SkillService *SkillService::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new SkillService();
        //if(m_instance->Init() != EXIT_SUCCESS)
        //{
        //    delete m_instance;
        //    m_instance = nullptr;
        //}
    }
    return m_instance;
}


std::optional<SkillDef> SkillService::GetOrLoadSkill(std::string skill_id)
{
    auto it = m_skills.find(skill_id);

    if(it != m_skills.end())  
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] alreay skill is Loaded", __FUNCTION__, __LINE__);
        return it->second;
    }

    K_slog_trace(K_SLOG_TRACE, "[%s][%d] Skill Load Start", __FUNCTION__, __LINE__);

    SkillDef skillDef;

    std::string path = SKILL_PATH + skill_id +".json";
    std::ifstream file(path);

    if(!file.is_open()) {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return std::nullopt;
    }

    // JSON 파일 파싱
    nlohmann::json j;
    file >> j;

    // Json 파일에서 몬스터 데이터 읽어오기
    LoadSkill(j, skillDef);

    m_skills[skill_id] = skillDef;

    return skillDef;

}

void SkillService::LoadSkill(nlohmann::json& j, SkillDef& skillDef)
{
    skillDef.skill_id = j["skill_id"];
	skillDef.key = j["mapId"];
	skillDef.type =j["type"];
    skillDef.cooldown_ms =j["cooldown_ms"];
    skillDef.mp_cost = j["mp_cost"];

    // Skill Json 파일에서 Hit 배열의 정보들을 반복문을 통해 설정
    for(const auto& hitData : j["hit"])
    {
        skillDef.hit.shape = hitData["shape"];
        skillDef.hit.range = hitData["range"];
        skillDef.hit.angle_deg = hitData["angle_deg"];
        skillDef.hit.max_targets = hitData["max_targets"];
        skillDef.hit.hit_count = hitData["hit_count"];
    }

    // Skill Json 파일에서 damage 배열의 정보들을 반복문을 통해 설정
    for(const auto& damange : j["damage"])
    {
        skillDef.damage.multiplier = damange["multiplier"];
        skillDef.damage.flat_add = damange["flat_add"];
    }

    // Skill Json 파일에서 Skill 배열의 정보들을 반복문을 통해 설정
    for(const auto& require : j["requirements"])
    {
        skillDef.Requirements.root_job = require["root"];
        skillDef.Requirements.min_tier= require["min_tier"];
        skillDef.Requirements.min_skill_level= require["min_skill_levelrequires"];
    }

     // Skill Json 파일에서 effect 배열의 정보들을 반복문을 통해 설정
    for(const auto& effects : j["effects"])
    {
        EffectDef effect;
        effect.type = effects["type"];
        effect.value = effects["force"];
        skillDef.effects.emplace_back(effect);
    }
    
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] SkillLoad Success", __FUNCTION__, __LINE__);

}



/*
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

    int cooldown_ms;
    int mp_cost;

    std::vector<EffectDef> effects;
};


*/