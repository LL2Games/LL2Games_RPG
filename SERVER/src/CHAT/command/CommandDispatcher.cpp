#include "CommandDispatcher.h"
#include <memory>
#include "FindUserCommand.h"

CommandDispatcher::CommandDispatcher()
{
    // /찾기
    this->Add(std::make_unique<FindUserCommand>(m_sender));
}

void CommandDispatcher::Add(std::unique_ptr<ICommand> cmd)
{
    m_commands.push_back(std::move(cmd));
}

bool CommandDispatcher::Dispatch(const std::string& msg)
{
    for (auto& cmd : m_commands)
    {
        if (cmd->Match(msg))
        {
            cmd->Execute(msg);
            return true;
        }
    }

    return false; //일반채팅
}