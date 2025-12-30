#include "CHANNEL/core/Player.h"

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


