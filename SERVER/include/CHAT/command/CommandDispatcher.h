#pragma once

#include <memory>
#include <vector>
#include "ICommand.h"
#include "ChannelCommandSender.h"

class CommandDispatcher
{
public:
    CommandDispatcher();
    void Add(std::unique_ptr<ICommand> cmd);
    bool Dispatch(const std::string& msg);

private:
    std::vector<std::unique_ptr<ICommand>> m_commands;
    ChannelCommandSender m_sender;
};