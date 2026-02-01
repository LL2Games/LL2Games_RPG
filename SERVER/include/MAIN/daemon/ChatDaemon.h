#pragma once
#include "BaseDaemon.h"

//Template Method

class ChatDaemon : public BaseDaemon {
public:
    ChatDaemon() : BaseDaemon("CHAT_DAEMON", "./chatD") {}
    ChatDaemon(const int flag) : BaseDaemon("CHAT_DAEMON", "./chatD", flag) {}
protected:
    const char* GetExecPath() const override
    {
        return "./chatD";
    }
};