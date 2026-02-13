#include "Player.h"
#include "timeUtility.h"
#include "ItemManager.h"


Player::Player() : m_char_id(0), m_name(""), m_cur_hp(30), m_max_hp(100)
{
}

Player::~Player()
{
}

void Player::SetInitData(const PlayerInitData playerInitData)
{
    /*    
        int char_id;
        std::string account_id;
        std::string name;
        int level;
        int job;
        int map_id;
        float xPos;
        float yPos; 
        */
    this->m_char_id = playerInitData.char_id;
    this->m_account_id = playerInitData.account_id;
    this->m_name = playerInitData.name;
    this->m_level = playerInitData.level;
    this->m_job = playerInitData.job;
    this->m_root_job = ToRootJob(playerInitData.root_job);
    this->m_map_id = playerInitData.map_id;
    this->m_xPos = playerInitData.xPos;
    this->m_yPos = playerInitData.yPos;

}

void Player::SetInitData(const PlayerInitData playerInitData, const CharacterStat &stat)
{
    /*    
        int char_id;
        std::string account_id;
        std::string name;
        int level;
        int job;
        int map_id;
        float xPos;
        float yPos; 
        */
    this->m_char_id = playerInitData.char_id;
    this->m_account_id = playerInitData.account_id;
    this->m_name = playerInitData.name;
    this->m_level = playerInitData.level;
    this->m_job = playerInitData.job;
    this->m_root_job = ToRootJob(playerInitData.root_job);
    this->m_map_id = playerInitData.map_id;
    this->m_xPos = playerInitData.xPos;
    this->m_yPos = playerInitData.yPos;
    this->m_stat = stat;


#if 1 /*ITEM 사용 테스트를 위한 Inven 채우기 */
    m_inven[2000000] = 100;
#endif
}


bool Player::CanAttack(SkillDef* skillDef)
{
    // 먼저 플레이어가 공격 가능한 상태인지 확인한다.
    if(m_CurrentState == PlayerState::STUNNED) 
    {   
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 플레이어가 공격 가능한 상태가 아닙니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
   
    // 스킬의 키 값을 통해서 해당 플레이어의 직업과 연관있는지 확인한다.

    // 플레이어의 출신과 스킬 사용 가능 출신이 동일한지 확인
    if(m_root_job != skillDef->Requirements.root_job)
    {
         K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 플레이어가 사용 가능한 스킬이 아닙니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    // 플레이어가 해당 스킬을 배웠는지 확인한다.
    auto skillit = m_learnedSkills.find(skillDef->skill_id);
    
    if(skillit == m_learnedSkills.end())
    {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 플레이어가 배우지 않은 스킬입니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    // 플레이어의 마나가 충분한지 확인한다.
    if(m_cur_mp < skillDef->mp_cost)
    {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 마나가 부족합니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    // 쿨타임 여부를 확인한다.
    auto coolit = skillCooldownEndMs.find(skillDef->skill_id);

    if(coolit == skillCooldownEndMs.end())
        return true;


    // 임시로 500으로 설정 나중에 변경 필요
    int64_t now = NowMs();
    if(now <= coolit->second)
    {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 아직 쿨타임입니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    skillCooldownEndMs[skillDef->skill_id] = now + skillDef->cooldown_ms;

    return true;


}


bool Player::CanUseItem(int item_id, int useCount)
{
    if (useCount <= 0) {
        K_slog_trace(K_SLOG_ERROR, "[%s:%d] invalid useCount=%d\n", __FUNCTION__, __LINE__, useCount);
        return false;
    }

    // 아이템 정의 존재/타입 체크
    const ItemInitData* def = ItemManager::GetInstance()->Find(item_id);

    if (!def) {
        K_slog_trace(K_SLOG_ERROR, "[%s:%d] unknown item_id=%d\n", __FUNCTION__, __LINE__, item_id);
        return false;
    }
    if (def->type != "consumable") {
        K_slog_trace(K_SLOG_ERROR, "[%s:%d] not consumable item_id=%d type=%s\n",
            __FUNCTION__, __LINE__, item_id, def->type.c_str());
        return false;
    }

    auto it = m_inven.find(item_id);
    if (it == m_inven.end()) {
        K_slog_trace(K_SLOG_ERROR, "[%s:%d] item not owned item_id=%d\n", __FUNCTION__, __LINE__, item_id);
        return false;
    }

    if (it->second < useCount) {
        K_slog_trace(K_SLOG_ERROR, "[%s:%d] not enough item_id=%d have=%d need=%d\n",
            __FUNCTION__, __LINE__, item_id, it->second, useCount);
        return false;
    }

    return true;
}

bool Player::UseItem(int itemId, int useCount)
{
   // 방어(실수 방지)
    if (useCount <= 0) return false;

    const ItemInitData* def = ItemManager::GetInstance()->Find(itemId);
    if (!def) return false;

    auto it = m_inven.find(itemId);
    if (it == m_inven.end()) return false;

    // 수량 차감
    it->second -= useCount;
    if (it->second <= 0) {
        m_inven.erase(it);
    }

    // 효과 적용
    // (ItemInitData.use_effect 타입에 따라 아래 분기만 맞추면 됨)
    if (def->use_effect)
    {
        // 예: hp/mp 회복
        const int hpAdd = def->use_effect->hp_restore * useCount;
        const int mpAdd = def->use_effect->mp_restore * useCount;

        AddHP(hpAdd);
        AddMP(mpAdd);

        // 쿨타임도 쓸 거면 여기서 등록
        // SetItemCooldown(itemId, def->use_effect->cooldown_ms);
    }

    return true;

}

int Player::GetItemCount(int itemId) const
{
    auto it = m_inven.find(itemId);
    return (it == m_inven.end()) ? 0 : it->second;
}

int Player::GetSkillLevel(std::string skill_id) const
{
    auto it = m_learnedSkills.find(skill_id);

    if(it == m_learnedSkills.end())
    {
        K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 배우지 않은 스킬입니다.\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    return it->second;
}


void Player::AddHP(int HP)
{
    K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 현재 체력 [%d].\n", __FILE__, __FUNCTION__, __LINE__,m_cur_hp);
    K_slog_trace(K_SLOG_TRACE, "[%s : %s][%d] 추가 체력 [%d].\n", __FILE__, __FUNCTION__, __LINE__,HP);
    m_cur_hp += HP;
    if (m_cur_hp > m_max_hp) m_cur_hp = m_max_hp;
    if (m_cur_hp < 0) m_cur_hp = 0;
}

void Player::AddMP(int MP)
{
    m_cur_mp += MP;
    if (m_cur_mp > m_max_mp) m_cur_mp = m_max_mp;
    if (m_cur_mp < 0) m_cur_mp = 0;
}
