#include "StatPacketFactory.h"

StatInfoPacket StatPacketFactory::MakeStatInfo(const CharacterStat& stat)
{
    StatInfoPacket pkt;
    auto &base = stat.GetBase();
    auto &derived = stat.GetDerived();

    pkt.str = base.str;
    pkt.dex = base.dex;
    pkt.intel = base.intel;
    pkt.luck = base.luck;

    pkt.maxHp = derived.maxHp;
    pkt.maxMp = derived.maxMp;

    pkt.curHp = stat.GetCurHp();
    pkt.curMp = stat.GetCurMp();

    pkt.remainAp = stat.GetRemainAp();

    return pkt;
}