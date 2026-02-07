#include "CharacterStat.h"
#include "K_slog.h"

void CharacterStat::Up(const std::string &statType)
{
    int statNum = 0;
    const static std::string stats[4] = {"str", "dex", "intel", "luck"};
    for (int i = 0; i < 4; i++)
    {
        if (stats[i] == statType)
        {
            statNum = i;
            break;
        }
    }

    switch (statNum)
    {
    case E_STR:
        m_base.str++;
        break;

    case E_DEX:
        m_base.dex++;
        break;

    case E_INT:
        m_base.intel++;
        break;

    case E_LUCK:
        m_base.luck++;
        break;
    }

    K_slog_trace(K_SLOG_DEBUG, "[%s(%s)][%d] stat[%s] up", __FILE__, __FUNCTION__, __LINE__, statType);
}