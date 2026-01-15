#pragma once
#include "BaseDaemon.h"

//Template Method

class ChannelDaemon : public BaseDaemon {
public:
    ChannelDaemon() : BaseDaemon("CHANNEL_DAEMON", "./channelD") {}
protected:
    const char* GetExecPath() const override
    {
        return "./channelD";
    }
};