#pragma once
#include "BaseDaemon.h"

//Template Method

class WorldDaemon : public BaseDaemon {
public:
    WorldDaemon() : BaseDaemon("WORLD_DAEMON", "./worldD") {}
protected:
    const char* GetExecPath() const override
    {
        return "./worldD";
    }
};