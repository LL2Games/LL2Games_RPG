#include "DaemonFactory.h"

std::unique_ptr<BaseDaemon> DaemonFactory::Create(DaemonType type, const int flag)
{
    switch (type)
    {
    case DaemonType::LOGIN:
        return std::make_unique<LoginDaemon>();
    case DaemonType::WORLD:
        return std::make_unique<WorldDaemon>();
    case DaemonType::CHAT:
        return std::make_unique<ChatDaemon>(flag);
    case DaemonType::CHANNEL:
        return std::make_unique<ChannelDaemon>(flag);
    }

    return nullptr;
}

std::unique_ptr<BaseDaemon> DaemonFactory::Create(DaemonType type, const std::string& configPath, const int flag)
{
    switch (type)
    {
    case DaemonType::LOGIN:
        return std::make_unique<LoginDaemon>(configPath);
    case DaemonType::WORLD:
        return std::make_unique<WorldDaemon>(configPath);
    case DaemonType::CHAT:
        return std::make_unique<ChatDaemon>(configPath, flag);
    case DaemonType::CHANNEL:
        return std::make_unique<ChannelDaemon>(configPath, flag);
    }

    return nullptr;
}