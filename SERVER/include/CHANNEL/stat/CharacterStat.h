#pragma once

struct BaseStat
{
    int str;
    int dex;
    int intel;
    int luck;
};

struct DerivedStat
{
    int maxHp;
    int maxMp;
};

class CharacterStat {

public:
    CharacterStat(){}
    CharacterStat(const BaseStat& base, const DerivedStat& derived, int curHp, int curMp, int remainAp)
    :m_base(base), m_derived(derived), m_cur_hp(curHp), m_cur_mp(curMp), m_remain_ap(remainAp)
    {}

    //조회용 인터페이스
    const BaseStat& GetBase() const { return m_base; }
    const DerivedStat& GetDerived() const {return m_derived; }
    int GetCurHp() const {return m_cur_hp;}
    int GetCurMp() const {return m_cur_mp;}
    int GetRemainAp() const {return m_remain_ap;}

private:
    BaseStat m_base;
    DerivedStat m_derived;
    int m_cur_hp;
    int m_cur_mp;
    int m_remain_ap;
};