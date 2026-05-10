#include "BaseDaemon.h"

class LoginDaemon : public BaseDaemon {
public:
    LoginDaemon() : BaseDaemon("LOGIN_DAEMON", "./bin/login_server") {}
protected:
    const char* GetExecPath() const override
    {
        return "./bin/login_server";
    }
};