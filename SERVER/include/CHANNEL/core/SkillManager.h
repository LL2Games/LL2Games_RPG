#pragma once

#include "common.h"
#include <optional>
#include <nlohmann/json.hpp>
#include <fstream>
#include "Skill_Info.h"
#include "utility.h"


class SkillManager
{
public:
    SkillManager(){};
    ~SkillManager(){};

    static SkillManager *GetInstance();

    bool Init();
    bool PreLoadAll();
    bool LoadJsonFile(const std::string& path, SkillDef& skillDef);
    void LoadSkill(nlohmann::json& j, SkillDef& skillDef);
    std::optional<SkillDef> GetSkill(int skill_id);

public:

private:
    std::unordered_map<int, SkillDef> m_skills;
    

    static SkillManager* m_instance;
};