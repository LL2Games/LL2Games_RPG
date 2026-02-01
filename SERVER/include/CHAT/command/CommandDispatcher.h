#pragma once

#include <memory>
#include <vector>
#include "ICommand.h"
#include "CommandMsgQueue.h"

class CommandDispatcher
{
public:
    CommandDispatcher(const int msgKey, const int typeSend, const int typeRecv);
    void Add(std::unique_ptr<ICommand> cmd);
    bool Dispatch(const std::string& msg);

private:
    std::vector<std::unique_ptr<ICommand>> m_commands;
    CommandMsgQueue m_mq;
};