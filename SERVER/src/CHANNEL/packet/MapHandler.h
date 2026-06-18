#include "IPacketHandler.h"
#include "common.h"
#include "MapService.h"


class MapHandler : public IPacketHandler
{
public:
    void Execute(PacketContext * ctx) override;
};
