#pragma once

#include "ICommand.h"
#include "ChannelCommandSender.h"

class FindUserCommand : public ICommand
{
public:
    FindUserCommand(ChannelCommandSender& sender);
    bool Match(const std::string& msg) const override;
    void Execute(const std::string &msg);

private:
    ChannelCommandSender& m_sender;
};