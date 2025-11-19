#include "BaseDaemon.h"
#include <fcntl.h>

BaseDaemon::BaseDaemon(const std::string &name, const std::string& path)
    : m_name(name), m_path(path)
{
}

void BaseDaemon::Run()
{
    pid_t pid = fork();
    if (pid < 0) return;

    if (pid > 0) //parent
    {
        m_pid = pid;
        return;
    }

    if (pid == 0) //child
    {
        m_pid = getpid();

        // 🔥 FD 닫기
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        // 🔥 /dev/null로 연결
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_RDWR);
        open("/dev/null", O_RDWR);

        printf("daemon Start[%s] pid=%d\n", m_name.c_str(), (int)m_pid);

        execl(GetExecPath(), GetExecPath(), NULL);
        perror("execl");
        _exit(127);
    }

}