#pragma once
#include "BaseDaemon.h"

//Template Method

class WorldDaemon : public BaseDaemon {
public:
    WorldDaemon() : BaseDaemon("WORLD_DAEMON", "./worldD") {}
    WorldDaemon(const std::string& configPath ) : BaseDaemon("WORLD_DAEMON", "./worldD", configPath) {}
protected:
    const char* GetExecPath() const override
    {
        return "./worldD";
    }
};