#include "K_slog.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <algorithm>

int g_logLevel; 

int K_slog_init(const char* path, const char *fileName, int logLevel)
{
	if (logLevel <= 0)
		return 0;

	g_logLevel = std::max(logLevel, (int)K_SLOG_NONE);
	g_logLevel = std::min(logLevel, (int)K_SLOG_TRACE);

    slog_init(fileName, SLOG_FLAGS_ALL, 0);

    slog_config_t config;
    slog_config_get(&config);
    config.nToFile = 1;
    config.eDateControl = SLOG_DATE_FULL;
	config.nKeepOpen = 0;
	config.nToScreen = 0;	//화면(콘솔) 로그 출력 OFF
	strncpy(config.sFilePath, path, sizeof(config.sFilePath) -1);
    slog_config_set(&config);
    
	return 0;
}
int K_slog_close()
{
    slog_destroy();
	return 0;
}

int K_slog_trace(enum e_slog lev, const char* pszFmt, ...)
{
	char* pBuf = NULL;
	int nLen = 0;
	va_list args;

	//로그레벨 0일경우 로그출력 X, TRACE는 로그레벨 1이상부터 출력
	if (g_logLevel == 0 || (lev != K_SLOG_TRACE && g_logLevel < lev))
		return -1;

	va_start(args, pszFmt);
	nLen = vsnprintf(NULL, 0, pszFmt, args);
	va_end(args);

	if (nLen < 0)
		return -1;

	pBuf = (char*)calloc(nLen + 1, sizeof(char));
	if (pBuf == NULL)
		return -1;

	va_start(args, pszFmt);
	vsnprintf(pBuf, nLen + 1, pszFmt, args);
	va_end(args);

	switch (lev)
	{
	case K_SLOG_DEBUG:
		slog_display(SLOG_DEBUG, 1, pBuf);
		break;
	/*case K_SLOG_WARN:
		slog_display(SLOG_WARN, 1, pBuf);
		break;*/
	case K_SLOG_ERROR:
		slog_display(SLOG_ERROR, 1, pBuf);
		break;

    default:
	case K_SLOG_TRACE:
		slog_display(SLOG_TRACE, 1, pBuf);
		break;

	}


	free(pBuf);

	return 0;
}