#pragma once
#include "BaseDaemon.h"

//Template Method

class ChatDaemon : public BaseDaemon {
public:
    ChatDaemon() : BaseDaemon("CHAT_DAEMON", "./chatD") {}
    ChatDaemon(const int flag) : BaseDaemon("CHAT_DAEMON", "./chatD", flag) {}
    ChatDaemon(const std::string& configPath, const int flag) : BaseDaemon("CHAT_DAEMON", "./chatD", configPath, flag) {}
protected:
    const char* GetExecPath() const override
    {
        return "./chatD";
    }
};