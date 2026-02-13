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
    int CalculateSkillBaseDamage(const Player* attacker, const SkillDef& skillDef); 

    int GetLevelDiffRate(int attackerLevel, int monsterLevel) const;
    // 함수 뒤에 const를 붙이면 멤버 변수를 변경하지 않겠다는 의미 : 함수 안에서 멤버 변수 값을 변경한다면 에러가 발생
    int CalculateFinalDamage(int baseSkillDmg, const Player* Attacker, const SkillDef& skillDef, const Monster& m) const;

    std::vector<Monster*> ComputeHitMonsters(Player* attacker, const std::vector<Monster>& monsters, const SkillDef& skillDef, std::string attack_dir);
private:
    SkillService* m_skillService;


};