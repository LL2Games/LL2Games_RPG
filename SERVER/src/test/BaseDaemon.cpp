#include "BaseDaemon.h"

BaseDaemon::BaseDaemon(const std::string &name, const std::string& path)
    : m_name(name), m_path(path)
{
}

void BaseDaemon::Run()
{
    pid_t pid = fork();
    if (pid < 0) return;

    if (pid == 0) //child
    {
        pid = fork();
        if (pid < 0) return;
        if (pid > 0) exit(0);

        m_pid = getpid();
        printf("[%s][%d] daemon Start pid=%d\n", m_name.c_str(), m_pid);

        execl(GetExecPath(), GetExecPath(), NULL);
        exit(0);
    }
}