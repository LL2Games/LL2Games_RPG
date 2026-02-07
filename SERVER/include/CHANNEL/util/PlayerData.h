#pragma once
#include <string>

struct PlayerInitData{
    int char_id;
    std::string account_id;
    std::string name;
    int level;
    int job;
    int map_id;
    float xPos;
    float yPos;
};


// 임시로 여기에 구현 /*2026.02.07 LJH */
// 플레이어가 스킬을 사용했을 때 Hit 당한 몬스터들을 관리하기 위한 구조체
struct Cand {
    Monster m;
    float front;   // dx * facingSign
};