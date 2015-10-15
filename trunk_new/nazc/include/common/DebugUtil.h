//////////////////////////////////////////////////////////////////////
//
//	DebugUtil.h: interface for the Debug Utility Functions.
//
//	This file is a part of the AIMIR-GCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEBUG_UTIL_H__
#define __DEBUG_UTIL_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "typedef.h"


#define ANSI_COLOR_BLACK        "\033[0;30m"
#define ANSI_COLOR_RED          "\033[0;31m"
#define ANSI_COLOR_GREEN        "\033[0;32m"
#define ANSI_COLOR_YELLOW       "\033[0;33m"
#define ANSI_COLOR_BLUE         "\033[0;34m"
#define ANSI_COLOR_MAGENTA      "\033[0;35m"
#define ANSI_COLOR_CYAN         "\033[0;36m"
#define ANSI_COLOR_WHITE        "\033[0;37m"

#define ANSI_BACK_BLACK         "\033[0;40m"
#define ANSI_BACK_RED           "\033[0;41m"
#define ANSI_BACK_GREEN         "\033[0;42m"
#define ANSI_BACK_YELLOW        "\033[0;43m"
#define ANSI_BACK_BLUE          "\033[0;44m"
#define ANSI_BACK_MAGENTA       "\033[0;45m"
#define ANSI_BACK_CYAN          "\033[0;46m"
#define ANSI_BACK_WHITE         "\033[0;47m"

#define ANSI_NORMAL             "\033[0m"

#if defined(__DEBUG_PTHREAD__)
#include <pthread.h>
#define __PTHREAD_INFO__        XDEBUG(ANSI_COLOR_BLUE " %s:%d PID=%d, THREAD ID=%d\r\n", __FILE__, __LINE__, getpid(), pthread_self())
#else
#define __PTHREAD_INFO__             
#endif

BOOL IS_DEBUG(void);
void SET_DEBUG_MODE(int nMode);
void SET_DEBUG_FILE(FILE * fp);
void DEBUG_FLUSH(void);
void XDEBUG(const char *fmt, ...);
void XDUMP(const char *pszBuffer, int nLength, int bPrintAscii=0, BOOL bDirect=FALSE);
//void XDUMPLONG(const char *pszBuffer, int nLength);

//void DUMPSTRING(const char *pszBuffer, int nLength);
void FDUMP(FILE *pFile, char *pszBuffer, int nLength, BOOL bPrintAscii=FALSE);
void FILEDUMP(const char *szFileName, char *pszBuffer, int nLength, BOOL bPrintAscii, BOOL bNewLine);

void CheckLogLimit(char *pszPath, int nSize);
void UpdateLogFile(const char * szLogDir, const char * szLogFile, int nLimitSize, BOOL bTime, const char *fmt, ...);

extern	int		m_nDebugLevel;

#endif	// __DEBUG_UTIL_H__
