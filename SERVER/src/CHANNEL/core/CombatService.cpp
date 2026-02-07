#include "CombatService.h"
#include "CommonEnum.h"
#include "Math.h"
#include "PlayerData.h"
#include "MapInstance.h"

CombatService::CombatService()
{
    m_skillService = SkillService::GetInstance();
}


int CombatService::HandleAttack(Player* Attacker, std::string skill_id, std::string attack_dir)
{
    MapInstance* map = Attacker->GetCurrentMap();

    std::optional<SkillDef> skillDef = m_skillService->GetOrLoadSkill(skill_id);

    if(!skillDef) return 0;

    //캐릭터가 공격 가능 상태인지 확인
    if(!Attacker->CanAttack(&*skillDef))
    {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 플레이어가 공격 가능한 상태가 아닙니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    // MapInstance에서 해당 맵에 스폰된 몬스터들의 정보를 추출
    std::vector<Monster> monster_list = map->GetMonsterList();
     
    // ComputeHitMonsters(Player* attacker, const std::vector<Monster>& monsters, const SkillDef& skillDef, std::string attack_dir)
    // 몬스터들 중에서 피격된 몬스터들을 가져온다.
    std::vector<Monster*> HitMonsters = ComputeHitMonsters(Attacker, monster_list, *skillDef, attack_dir);
    
    map->ResolveSkillHit(Attacker, *skillDef, HitMonsters);
  
    return 1;

}

/*
bool IsHitFrontBox2D(
    const Vec2& attackerPos,
    int facingSign,            // -1: Left, +1: Right (공격자 기준)
    const Vec2& targetPos,
    float range,               // 예: 120
    float verticalTolerance    // 예: 60 (스킬별 튜닝)
)
*/


std::vector<Monster*> CombatService::ComputeHitMonsters(Player* attacker, const std::vector<Monster>& monsters, const SkillDef& skillDef, std::string attack_dir)
{
    int hit_count = 0;
    Vec2 attackerPos;
    std::vector<Cand> CanHitMonsters;
    std::vector<Monster*> HitMonsters;

    CanHitMonsters.reserve(monsters.size());

    

    // 공격 방향에 따라서 곱해주는 값 설정
    int dir = attack_dir == "Left" ? -1 : 1;

    attackerPos = attacker->GetPosition();

    for (Monster m : monsters)
    {
        Vec2 mp = m.GetPos();

        // 스킬 범위에 해당 몬스터가 있는지 없는 확인
        if (!IsHitFrontBox2D(attackerPos, dir, mp, skillDef.hit.range, skillDef.hit.angle_deg))
            continue;

        float dx = mp.xPos - attackerPos.xPos;
        float front = dx * (float)dir;

        CanHitMonsters.push_back({m, front});
    }
    
    std::sort(CanHitMonsters.begin(), CanHitMonsters.end(),
        [](const Cand& a, const Cand& b)
        {
            // float은 오차가 있으니, front가 사실상 같은 경우는 instanceId로 묶는다
            if (a.front < b.front) return true;
            if (a.front > b.front) return false;
            return a.m.GetInstanceId() < b.m.GetInstanceId();
        });

    int take = std::min<int>(skillDef.hit.max_targets, (int)CanHitMonsters.size());
    HitMonsters.reserve(take);

    for(int i =0; i<take; ++i)
    {
        HitMonsters.push_back(&CanHitMonsters[i].m);
    }

    return HitMonsters;

}