#pragma once
#include <memory>
#include "LoginDaemon.h"
#include "ChatDaemon.h"
#include "WorldDaemon.h"

//Factory

enum class DaemonType { LOGIN, CHAT, WORLD };

class DaemonFactory
{
public:
    static std::unique_ptr<BaseDaemon> Create(DaemonType type);
};
