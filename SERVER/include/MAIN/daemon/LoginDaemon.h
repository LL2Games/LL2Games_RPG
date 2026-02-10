#pragma once
#include "BaseDaemon.h"

//Template Method

class LoginDaemon : public BaseDaemon {
public:
    LoginDaemon() : BaseDaemon("LOGIN_DAEMON", "./loginD") {}
protected:
    const char* GetExecPath() const override
    {
        return "./loginD";
    }
};