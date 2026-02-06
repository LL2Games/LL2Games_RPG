#include "CombatService.h"

CombatService::CombatService()
{
    m_skillService = SkillService::GetInstance();
}


int CombatService::HandleAttack(Player* Attacker, std::string skill_id, std::string attack_dir)
{

    std::optional<SkillDef> skillDef = m_skillService->GetOrLoadSkill(skill_id);

    if(!skillDef) return 0;

    //캐릭터가 공격 가능 상태인지 확인
    if(!Attacker->CanAttack(&*skillDef))
    {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 플레이어가 공격 가능한 상태가 아닙니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    Attacker->GetCurrentMap()->ResolveSkillHit(Attacker, &*skillDef, attack_dir);
        
    (void)attack_dir;





    

    return 1;

}