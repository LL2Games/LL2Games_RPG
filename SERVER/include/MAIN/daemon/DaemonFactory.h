#pragma once
#include <memory>
#include "LoginDaemon.h"
#include "ChatDaemon.h"

//Factory

enum class DaemonType { LOGIN, CHAT };

class DaemonFactory
{
public:
    static std::unique_ptr<BaseDaemon> Create(DaemonType type);
};
