#pragma once
#include "BaseDaemon.h"

//Template Method

class ChatDaemon : public BaseDaemon {
public:
    ChatDaemon() : BaseDaemon("CHAT_DAEMON", "./bin/chatD") {}
protected:
    const char* GetExecPath() const override
    {
        return "./bin/chatD";
    }
};