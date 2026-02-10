#pragma once
#include "common.h"
#include "CommonEnum.h"
#include "util/PlayerData.h"
#include "CharacterStat.h"


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

private:
    int m_char_id;
    std::string m_account_id;
    std::string m_name;
    int m_level;
    int m_job;
    int m_map_id;
    float m_xPos;
    float m_yPos;

    MapInstance* m_current_map;
    ChannelSession* m_session;
private:
    bool m_isChangedInventory;
    bool m_isChangedStatas;
    bool m_isChangedPositon;
    CharacterStat m_stat;
   

};
