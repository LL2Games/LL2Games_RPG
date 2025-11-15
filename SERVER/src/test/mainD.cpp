#include "common.h"
#include "UTIL.h"
#define DAEMON_NAME "MAIN_SERVER"
#define CHAT_DAEMON "./chat_server"
#define LOGIN_DAEMON "./login_server"


//log
#include "log.h"
#define LOG_PATH "./log/main"

int DoDaemon()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d]Error\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    if (pid == 0) // 자식프로세스(데몬)
    {
        return 0;
    }

    K_slog_trace(K_SLOG_TRACE, "[%d]Parent Main Process exit...\n", getpid());

    // 부모프로세스 종료
    exit(0);
}

pid_t StartDaemon(const char *path)
{
    K_slog_trace(K_SLOG_TRACE, "[%s] Start: %s\n", __FUNCTION__, path);
    pid_t pid = fork();
    if (pid < 0)
    {
        K_slog_trace(K_SLOG_TRACE, "[%s][%d] Error\n", __FUNCTION__, __LINE__);
        exit(1);
    }
    else if (pid == 0) // 자식프로세스
    {
        // // 자식 프로세스 (이제 데몬화 시작)
        // if (setsid() < 0)
        //     return -1;

        // 2차 fork()로 세션 리더가 다시 tty를 얻지 못하게 함
        pid = fork();
        if (pid < 0)
            return -1;
        if (pid > 0)
            exit(0);

        // umask(0);
        // chdir("/");

        // 표준 입출력 닫기
        // fclose(stdin);
        // fclose(stdout);
        // fclose(stderr);

        // /dev/null 로 리다이렉트
        // open("/dev/null", O_RDONLY);
        // open("/dev/null", O_WRONLY);
        // open("/dev/null", O_RDWR);

        K_slog_trace(K_SLOG_TRACE, "[StartDaemon][pid=%d] Start: %s\n", getpid(), path);

        execl(path, path, NULL);
        exit(0);
    }
    else
    {
        K_slog_trace(K_SLOG_TRACE, "[%s] End: %s\n", __FUNCTION__, path);
    }

    return pid;
}

int main()
{
    int MasterPid = 0;
    pid_t ChatDaemonPID = 0;
    pid_t LoginDaemonPID = 0;

    // 데몬으로 구동
    DoDaemon();

    //log
    K_slog_init(LOG_PATH, DAEMON_NAME);

    MasterPid = getpid();
    K_slog_trace(K_SLOG_TRACE, "MasterPid[%d]\n", MasterPid);

    LoginDaemonPID = StartDaemon(LOGIN_DAEMON);
    ChatDaemonPID = StartDaemon(CHAT_DAEMON);

    K_slog_trace(K_SLOG_TRACE, "[%s] Start PID: %d\n", LOGIN_DAEMON, LoginDaemonPID);
    K_slog_trace(K_SLOG_TRACE, "[%s] Start PID: %d\n", CHAT_DAEMON, ChatDaemonPID);
    while (true)
    {
        int fd;
        char proc[50];
        snprintf(proc, sizeof(proc), "/proc/%d", ChatDaemonPID);
        if ((fd = open(proc, O_RDONLY)) <= 0)
        {
            // 프로세스가 없을경우 재실행
            ChatDaemonPID = StartDaemon(CHAT_DAEMON);
        }
        sleep(1);
    }
    return 0;
}