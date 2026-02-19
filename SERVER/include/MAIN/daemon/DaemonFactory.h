#pragma once
#include <memory>
#include "LoginDaemon.h"
#include "ChatDaemon.h"
#include "WorldDaemon.h"
#include "ChannelDaemon.h"

//Factory

enum class DaemonType { LOGIN, CHAT, WORLD, CHANNEL };

class DaemonFactory
{
public:
    static std::unique_ptr<BaseDaemon> Create(DaemonType type, const int flag = 0);
};
