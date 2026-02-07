#include "common.h"
#include "SkillService.h"
#include "MapInstance.h"
#include "Player.h"
#include "Monster.h"


class CombatService
{
public:
    CombatService();
    ~CombatService();

    int HandleAttack(Player* Attacker, std::string skill_id, std::string attack_dir);

    std::vector<Monster*> ComputeHitMonsters(Player* attacker, const std::vector<Monster>& monsters, const SkillDef& skillDef, std::string attack_dir);
private:
    SkillService* m_skillService;


};