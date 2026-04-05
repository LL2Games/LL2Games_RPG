#pragma once
#include <string>
#include <unistd.h>

class BaseDaemon
{
public:
    virtual ~BaseDaemon() = default;
    void Run(); //Template Method;

    pid_t GetPID() const { return m_pid; }
    const std::string& GetName() const { return m_name; }

protected:
    BaseDaemon(const std::string& name, const std::string& path);
    virtual const char *GetExecPath() const = 0;

private:
    std::string m_name;
    std::string m_path;
    pid_t m_pid;

};