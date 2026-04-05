#include "SkillManager.h"
#include <string>


#define SKILL_PATH "../src/CHANNEL/data/Skills/"
namespace fs = std::filesystem;

SkillManager *SkillManager::m_instance =nullptr;


SkillManager *SkillManager::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new SkillManager();
        //if(m_instance->Init() != EXIT_SUCCESS)
        //{
        //    delete m_instance;
        //    m_instance = nullptr;
        //}
    }
    return m_instance;
}


bool SkillManager::Init()
{
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] Init Error", __FUNCTION__, __LINE__);
    if(!PreLoadAll()) return false;
    return true;
}

bool SkillManager::PreLoadAll()
{
   for (const auto& entry : fs::recursive_directory_iterator(SKILL_PATH))
    {
        if (!entry.is_regular_file())
            continue;
        if (entry.path().extension() != ".json")
            continue;

        /*
            // entry.path() -> 파일 경로
            // entry.path().filename() -> 파일명
            // entry.path().stem() -> 확장자 제거한 파일명(예: "1001")

        */
        int skill_id = 0;
        if(!utility::StringToInt(entry.path().stem().string(), skill_id)) return false;

        auto it = m_skills.find(skill_id);
        if (it != m_skills.end())
            continue;

        SkillDef skillDef;

        bool is_Load = LoadJsonFile(entry.path().string(), skillDef);
        if (!is_Load)
            return false;

        m_skills.emplace(skill_id, std::move(skillDef));
    }
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] Skill PreLoadAll Success", __FUNCTION__, __LINE__);
    return true;
}



std::optional<SkillDef> SkillManager::GetSkill(int skill_id)
{
    auto it = m_skills.find(skill_id);

    if(it != m_skills.end())  
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] alreay skill is Loaded", __FUNCTION__, __LINE__);
        return it->second;
    }

    return std::nullopt;

}


bool SkillManager::LoadJsonFile(const std::string& path, SkillDef& skillDef)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] FAILED OPEN [%s] FILE", __FUNCTION__, __LINE__, path.c_str());
        return false;
    }
    
    // JSON 파일 파싱
    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] JSON Error", __FUNCTION__, __LINE__);
        // JSON 문법 깨짐/파싱 실패
        return false;
    }

    if (j.is_null())
        return false;

 
    // Json 파일에서 Skill 데이터 읽어오기
    LoadSkill(j, skillDef);

    
    return true;
}

void SkillManager::LoadSkill(nlohmann::json& j, SkillDef& skillDef)
{
    K_slog_trace(K_SLOG_TRACE, "[%s][%d] LoadSkill Start", __FUNCTION__, __LINE__);
    skillDef.skill_id = j.at("skill_id").get<int>();
	skillDef.key = j.at("key").get<std::string>();

    skillDef.type = Skill::SetSkillType(j.at("type").get<std::string>());
  
    skillDef.cooldown_ms = j.at("cooldown_ms").get<int>();
    skillDef.mp_cost = j.at("mp_cost").get<int>();

    // Skill Json 파일에서 hit 배열의 정보들 가져옴
    const auto& hit = j.at("hit").at(0);

    skillDef.hit.shape = Skill::SetHitShape(hit.at("shape").get<std::string>());
    skillDef.hit.range = hit.at("range").get<float>();
    skillDef.hit.angle_deg = hit.at("angle_deg").get<float>();
    skillDef.hit.max_targets = hit.at("max_targets").get<int>();
    skillDef.hit.hit_count = hit.at("hit_count").get<int>();

    // Skill Json 파일에서 damage 배열의 정보들 가져옴
    const auto& damange = j.at("damage").at(0);

    skillDef.damage.multiplier = damange.at("multiplier").get<float>();
    skillDef.damage.flat_add = damange.at("flat_add").get<int>();

 
    // Skill Json 파일에서 requirements 배열의 정보들 가져옴
    const auto& requirements = j.at("requirements").at(0);

    skillDef.Requirements.root_job = PlayerData::ToRootJob(requirements.at("root_job").get<int>());
    skillDef.Requirements.min_tier= requirements.at("min_tier").get<int>();
    skillDef.Requirements.min_skill_level= requirements.at("min_skill_level").get<int>();;

    // Skill Json 파일에서 effects 배열의 정보들 가져옴   
    const auto& effectsArr = j.at("effects");
    if (!effectsArr.is_array()) return; // 방어

    skillDef.effects.clear();
    skillDef.effects.reserve(effectsArr.size());

    for (size_t i = 0; i < effectsArr.size(); ++i)
    {
        const auto& effects = effectsArr.at(i);

        EffectDef effect;
        effect.type  = Skill::SetEffectType(effects.at("type").get<std::string>());

        // 타입별로 키가 다를 수 있으니 value()로 기본값 주는 게 안전
        effect.value = effects.at("force").get<float>();

        skillDef.effects.push_back(effect);
    }


    K_slog_trace(K_SLOG_TRACE, "[%s][%d] SkillLoad Success Skill_id [%d]", __FUNCTION__, __LINE__, skillDef.skill_id);

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