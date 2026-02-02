#pragma once

struct BaseStat{};
struct DerivedStat{};

class CharacterStat {

public:
    CharacterStat(const BaseStat& base, const DerivedStat& derived, int curHp, int curMp, int remainAp)
    :;

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