#pragma once
#include "BaseDaemon.h"

class ChatDaemon : public BaseDaemon {
public:
    ChatDaemon() : BaseDaemon("CHAT_DAEMON", "./bin/chat_server") {}
protected:
    const char* GetExecPath() const override
    {
        return "./bin/chat_server";
    }
};