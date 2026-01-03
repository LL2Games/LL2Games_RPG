#include "DaemonFactory.h"

std::unique_ptr<BaseDaemon> DaemonFactory::Create(DaemonType type)
{
    switch (type)
    {
    case DaemonType::LOGIN:
        return std::make_unique<LoginDaemon>();
    case DaemonType::CHAT:
        return std::make_unique<ChatDaemon>();
    case DaemonType::WORLD:
        return std::make_unique<WorldDaemon>();
    }

    return nullptr;
}