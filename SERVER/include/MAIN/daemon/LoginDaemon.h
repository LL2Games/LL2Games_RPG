#pragma once
#include "BaseDaemon.h"

//Template Method

class LoginDaemon : public BaseDaemon {
public:
    LoginDaemon() : BaseDaemon("LOGIN_DAEMON", "./loginD") {}
    LoginDaemon(const std::string& configPath) : BaseDaemon("LOGIN_DAEMON", "./loginD", configPath) {}
protected:
    const char* GetExecPath() const override
    {
        return "./loginD";
    }
};