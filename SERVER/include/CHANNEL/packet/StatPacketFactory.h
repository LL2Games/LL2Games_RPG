#include "StatInfoPacket.h"
#include "CharacterStat.h"

class StatPacketFactory
{
public:
    static StatInfoPacket MakeStatInfo(const CharacterStat& stat);
};