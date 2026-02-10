#pragma once
#include <string>

//Command Pattern
class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual bool Match(const std::string& msg) const = 0;
    virtual void Execute(const std::string &msg) = 0;
};