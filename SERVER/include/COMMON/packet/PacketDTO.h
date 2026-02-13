#pragma once
#include <string.h>
#include <vector>

/* 
    Session Send 시 필요한 정보들을 저장하는 구조체들의 모음 헤더 파일
*/


// 플레이어가 아이템 사용 시 반환 구조체
struct UseItemResult {
    int result;
    int item_id;
    int used_count;
    int remain_count;
    int hp;
    int mp;
};