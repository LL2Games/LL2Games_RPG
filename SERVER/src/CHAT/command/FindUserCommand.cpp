#include "FindUserCommand.h"
#include "K_slog.h"

FindUserCommand::FindUserCommand(CommandMsgQueue& mq) : m_mq(mq)
{
}

bool FindUserCommand::Match(const std::string& msg) const
{
    return msg.rfind("/찾기 ", 0) == 0;
}

void FindUserCommand::Execute(const std::string &msg)
{
    m_mq.Send(msg);
}
