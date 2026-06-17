#pragma once

#include "ICommand.h"
#include "CommandMsgQueue.h"

#define MSG_COMMAND_SEND 0
#define MSG_COMMAND_RECV 1

class FindUserCommand : public ICommand
{
public:
    FindUserCommand(CommandMsgQueue& mq);
    bool Match(const std::string& msg) const override;
    void Execute(const std::string &msg);

private:
    CommandMsgQueue& m_mq;
};