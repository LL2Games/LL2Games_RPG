#include "DaemonFactory.h"

std::unique_ptr<BaseDaemon> DaemonFactory::Create(DaemonType type)
{
    switch (type)
    {
    case DaemonType::LOGIN:
        return std::make_unique<LoginDaemon>();
    case DaemonType::CHAT:
        return nullptr;
    }

    return nullptr;
}