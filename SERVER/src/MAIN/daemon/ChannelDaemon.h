#pragma once
#include "BaseDaemon.h"

//Template Method

class ChannelDaemon : public BaseDaemon {
public:
    ChannelDaemon() : BaseDaemon("CHANNEL_DAEMON", "./channelD") {}
    ChannelDaemon(const int flag) : BaseDaemon("CHANNEL_DAEMON", "./channelD", flag) {}
    ChannelDaemon(const std::string& configPath, const int flag) : BaseDaemon("CHANNEL_DAEMON", "./channelD", configPath, flag) {}
protected:
    const char* GetExecPath() const override
    {
        return "./channelD";
    }
};