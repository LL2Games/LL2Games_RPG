#include "slog.h"

enum e_slog
{
    K_SLOG_NONE = 0, // 로그 출력 없음
    K_SLOG_ERROR,    // 보안수준 lev = 1
    K_SLOG_DEBUG,    // 보안수준 lev = 2
    K_SLOG_TRACE,    // 보안수준 무시

    // K_SLOG_WARN,			//보안수준 lev = 2
};

int K_slog_init(const char* path, const char *fileName);
int K_slog_close();
int K_slog_trace(enum e_slog lev, const char *pszFmt, ...);