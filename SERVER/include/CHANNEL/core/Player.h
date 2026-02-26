#pragma once
#include "common.h"
#include "CommonEnum.h"
#include "util/PlayerData.h"
#include "CharacterStat.h"
#include "PlayerState.h"
#include "PlayerData.h"
#include "Skill_Info.h"
#include "time.h"
#include "Collider.h"

class ChannelSession;
class MapInstance;

class Player
{
public:
    Player();
    ~Player();

public: 
    void SetId(int m_char_id){this->m_char_id = m_char_id;}
    void SetAccountId(std::string m_account_id) {this->m_account_id = m_account_id;}
    void SetName(std::string m_name){ this->m_name = m_name;}
    
    void SetJob(int m_job){this->m_job = m_job;}
    void SetMapId(int m_map_id){this->m_map_id = m_map_id;}

    void SetLevel(int m_level){this->m_level = m_level;}

    void SetPos(float m_xPos, float m_yPos) {this->m_xPos = m_xPos; this->m_yPos = m_yPos;}\
    void SetPos(Vec2 Pos) {m_xPos = Pos.xPos, m_yPos = Pos.yPos;}

    void SetInitData(const PlayerInitData playerInitData);
    void SetInitData(const PlayerInitData playerInitData, const CharacterStat &stat);

    void SetCurrentMap(MapInstance* map) {m_current_map = map;}
    void SetSession(ChannelSession* session) {m_session = session;}

public:
    int GetCurHP(){return m_stat.GetCurHp();}
    int GetCurMP(){return m_stat.GetCurMp();}
    int GetId() {return m_char_id;}
    int GetLevel() const {return m_level;}

    bool IsAlive(){return m_CurrentState != PlayerState::DEAD ? true : false;}

    std::string GetName() {return m_name;}

    MapInstance* GetCurrentMap() {return m_current_map;}
    ChannelSession* GetSession() {return m_session;}

    //stat
    CharacterStat& GetStat() {return m_stat;}
    const CharacterStat& GetStat() const {return m_stat;}    
  
    Vec2 GetPos() {return Vec2{m_xPos, m_yPos};}
    RootJob GetRootJob() const {return m_root_job;}
    Collider2D GetCollider() {return m_collider;}

public:

    // 현재 플레이어가 공격 가능 상태인지 확인한다.
    bool CanAttack(SkillDef* skillDef);

    bool CanUseItem(int itemId, int useCount);

    int GetSkillLevel(int skill_id) const;

    void UseSkill(SkillDef* skillDef);
    bool UseItem(int itemId, int useCount);

    void AddHP(int HP);
    void AddMP(int MP);

    int GetItemCount(int itemId) const;

    // 현재 공격을 받을 수 있는 상태인지 확인하는 함수 : 메이플에서 피격 당할 시 1초 정도의 무적이 존재하기 때문에 확인 함수 필요
    bool CanTakeAnyContactDamage(int64_t nowMs);

    // 플레이어가 피격 당했을 때 처리하는 함수
    void OnDamaged(int dmg,int64_t nowMs);

    void Dead();

    
private:
    int m_char_id;
    std::string m_account_id;
    std::string m_name;
   
    RootJob m_root_job;

    int m_level;
    int m_job;
    int m_map_id;
    float m_xPos;
    float m_yPos;

    Collider2D m_collider;

    MapInstance* m_current_map;
    ChannelSession* m_session;

    PlayerState m_CurrentState;

    // 플레이어가 배운 스킬들 저장 string은 skillID, int는 스킬 레벨
    std::unordered_map<int , int> m_learnedSkills;
    // 사용한 스킬쿨들을 저장
    std::unordered_map<int, int64_t> skillCooldownEndMs;

    // 플레이어가 들고 있는 아이템 정보들
    std::unordered_map<int, int> m_inven;
private:
    bool m_isChangedInventory;
    bool m_isChangedStatas;
    bool m_isChangedPositon;
    CharacterStat m_stat;
   
    int64_t m_nextContactDamageAllowedMs;
    int64_t m_contactDamageCooldownMs;
};
