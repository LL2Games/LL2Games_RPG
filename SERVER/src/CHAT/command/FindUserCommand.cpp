#include "FindUserCommand.h"
#include "K_slog.h"

FindUserCommand::FindUserCommand(ChannelCommandSender& sender) : m_sender(sender)
{
}

bool FindUserCommand::Match(const std::string& msg) const
{
    return msg.rfind("/찾기 ", 0) == 0;
}

void FindUserCommand::Execute(const std::string &msg)
{
    m_sender.Send(msg);
}
