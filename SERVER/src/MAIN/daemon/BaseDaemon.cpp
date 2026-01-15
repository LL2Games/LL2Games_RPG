#include "BaseDaemon.h"
#include <fcntl.h>
#include <sys/wait.h>
#include "K_slog.h"

BaseDaemon::BaseDaemon(const std::string &name, const std::string& path)
    : m_name(name), m_path(path)
{
}

pid_t BaseDaemon::Run()
{
    int fd[2];
    pid_t pid;

    if (pipe(fd) < 0)
    {
        perror("pipe");
        return  -1;
    }

    pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) //child
    {
        close(fd[0]); //read close;
        pid_t pid2 = fork();
        if (pid2 < 0) _exit(127);
        if (pid2 > 0)
        {
            if (write(fd[1], (void *)&pid2, sizeof(pid2)) < 0)
            {
                perror("write");
                _exit(127);
            }
            close(fd[1]); //write close;
            _exit(0);
        }
        close(fd[1]); //write close;
        m_pid = getpid();

        // FD 닫기
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        // /dev/null로 연결
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_RDWR);
        open("/dev/null", O_RDWR);

        printf("daemon Start[%s] pid=%d\n", m_name.c_str(), (int)m_pid);

        execl(GetExecPath(), GetExecPath(), NULL);
        perror("execl");
        _exit(127);
    }

    if (pid > 0) //parent
    {
        pid_t pid2;
        close(fd[1]); //write close;
        if (read(fd[0], (void *)&pid2, sizeof(pid2)) < 0)
        {
            perror("read");
            _exit(127);
        }
        close(fd[0]); //read close;

        waitpid(pid, nullptr, 0);
        K_slog_trace(K_SLOG_DEBUG, "Daemon[%s] started pid=%d", m_name.c_str(), pid2);
        m_pid = pid2;
        return pid2;
    }

    return -1;
}