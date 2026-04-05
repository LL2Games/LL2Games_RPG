#pragma once

#include <iostream>
#include <memory>
#include "BaseDaemon.h"
#include "LoginDaemon.h"

enum class DaemonType
{
    LOGIN,
    CHAT,
};

class DaemonFactory
{
public:
    static std::unique_ptr<BaseDaemon> Create(DaemonType type);
};
