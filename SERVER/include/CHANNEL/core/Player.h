#pragma once
#include "common.h"
#include "CommonEnum.h"
#include "util/PlayerData.h"
#include "CharacterStat.h"
#include "PlayerState.h"
#include "Skill_Info.h"
#include "time.h"

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
    void SetPosition(float m_xPos, float m_yPos) {this->m_xPos = m_xPos; this->m_yPos = m_yPos;}\
    void SetPosition(Vec2 Pos) {m_xPos = Pos.xPos, m_yPos = Pos.yPos;}


    void SetInitData(const PlayerInitData playerInitData);
    void SetInitData(const PlayerInitData playerInitData, const CharacterStat &stat);

    void SetCurrentMap(MapInstance* map) {m_current_map = map;}
    void SetSession(ChannelSession* session) {m_session = session;}

    int GetId() {return m_char_id;}
    std::string GetName() {return m_name;}
    MapInstance* GetCurrentMap() {return m_current_map;}
    ChannelSession* GetSession() {return m_session;}

    //stat
    CharacterStat& GetStat() {return m_stat;}
    const CharacterStat& GetStat() const {return m_stat;}    
    Vec2 GetPosition() {return Vec2{m_xPos, m_yPos};}

public:

    // 현재 플레이어가 공격 가능 상태인지 확인한다.
    bool CanAttack(SkillDef* skillDef);


private:
    int m_char_id;
    std::string m_account_id;
    std::string m_name;
    std::string root_job;
    int m_level;
    int m_job;
    int m_map_id;
    float m_xPos;
    float m_yPos;

    int m_cur_hp;
    int m_cur_mp;

    int m_max_hp;
    int m_max_mp;

    MapInstance* m_current_map;
    ChannelSession* m_session;

    PlayerState m_CurrentState;

    // 플레이어가 배운 스킬들 저장 string은 skillID, int는 스킬 레벨
    std::unordered_map<std::string , int> m_learnedSkills;
    // 사용한 스킬쿨들을 저장
    std::unordered_map<std::string, int64_t> skillCooldownEndMs;
private:
    bool m_isChangedInventory;
    bool m_isChangedStatas;
    bool m_isChangedPositon;
    CharacterStat m_stat;
   

};
