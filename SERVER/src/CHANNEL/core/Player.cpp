#include "Player.h"
#include "timeUtility.h"


Player::Player() : m_char_id(0), m_name("")
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
    this->m_map_id = playerInitData.map_id;
    this->m_xPos = playerInitData.xPos;
    this->m_yPos = playerInitData.yPos;

    this->m_stat = stat;
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
    if(root_job != skillDef->Requirements.root)
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

