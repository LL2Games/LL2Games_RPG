#include "common.h"
#include <optional>
#include <nlohmann/json.hpp>
#include <fstream>
#include "Skill_Info.h"




class SkillService
{
public:
    SkillService(){};
    ~SkillService(){};

    static SkillService *GetInstance();

    void LoadSkill(nlohmann::json& j, SkillDef& skillDef);
    std::optional<SkillDef> GetOrLoadSkill(std::string skill_id);

public:


private:
    std::unordered_map<std::string, SkillDef> m_skills;
    

    static SkillService* m_instance;
};