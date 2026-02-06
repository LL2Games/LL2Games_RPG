#include "common.h"
#include "SkillService.h"
#include "MapInstance.h"
#include "Player.h"



class CombatService
{
public:
    CombatService();
    ~CombatService();

    int HandleAttack(Player* Attacker, std::string skill_id, std::string attack_dir);

private:
    SkillService* m_skillService;


};