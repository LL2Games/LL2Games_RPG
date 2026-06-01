#pragma once
#include "common.h"
struct StatInfoPacket
{
    int str;
    int dex;
    int intel;
    int luck;

    int maxHp;
    int maxMp;
    int curMp;
    int curHp;

    int remainAp;
};

struct ExpResult
{
    int64_t gainedExp = 0;

    int oldLevel = 0;
    int newLevel = 0;

    int64_t curExp = 0;
    int64_t needExp = 0;

    int remainAp = 0;

    int curHp = 0;
    int maxHp = 0;
    int curMp = 0;
    int maxMp = 0;

    bool levelUp = false;
};