//////////////////////////////////////////////////////////////////////
//
//	DebugUtil.cpp: implementation of Debugging Functions.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#ifndef _WIN32
#include "DebugServer.h"
#endif
#include "DebugUtil.h"
#include "Locker.h"

#include "logdef.h"

CLocker		m_DebugLocker;
//CLocker		m_LogLocker;
char		m_szPath[_MAX_PATH+1] = "";
#ifdef DEBUG
int			m_nDebugLevel = 0;
#else
int			m_nDebugLevel = 1;
#endif
FILE        *m_pDebugFile = stdout;

BOOL	m_bEnableCommandLog = TRUE;
int		m_nTimeLogSize = 100;

void SET_DEBUG_MODE(int nMode)
{
	m_nDebugLevel = nMode;
}

void SET_DEBUG_FILE(FILE * fp)
{
    m_pDebugFile = fp;
}

void DEBUG_FLUSH(void)
{
    fflush(m_pDebugFile);
}

BOOL IS_DEBUG(void)
{
	return (m_nDebugLevel != 0) ? FALSE : TRUE; 
}

///////////////////////////////////////////////////////////////////////////////////////////////
/** Log File Size를 검사하고 기준값 보다 크면 삭제한다.
  *
  * @param pszPath File Path
  * @param nSize 기준 size (단위 kb). 0일 경우 검사를 무시한다.
  */
void CheckLogLimit(char *pszPath, int nSize)
{
	struct 		stat file_info;
    
    if(!nSize) return;
	
	if(!stat(pszPath, &file_info))
    {
        if (file_info.st_size > (nSize*1024))
        {
            unlink(pszPath);
        }
    }
}

void UpdateLogFile(const char * szLogDir, const char * szLogFile, int nLimitSize, BOOL bTime, const char *fmt, ...)
{
	FILE		*fp;
	time_t		ltime=0;
	struct tm	*today;
	char		szPath[256];
	va_list	    ap;

	time(&ltime);
	today = localtime(&ltime);
	sprintf(szPath, "%s/%s%04d%02d%02d.log", szLogDir, szLogFile,
			today->tm_year+1900, today->tm_mon+1, today->tm_mday);

	m_DebugLocker.Lock();
    /** Write 전에 검사한다.. 
      * Write 후에 검사하면 현재 내용도 없어진다.
      */
    CheckLogLimit(szPath, nLimitSize);
	fp = fopen(szPath, "a+b");
	if (fp != NULL)
	{	
		if (bTime) 
        {
		    fprintf(fp, "%02d/%02d %02d:%02d:%02d ", 
                    today->tm_mon + 1, today->tm_mday,
                    today->tm_hour, today->tm_min, today->tm_sec);
        }
	    va_start(ap, fmt);
	    vfprintf(fp, fmt, ap);
	    va_end(ap);

		fclose(fp);
	}
	m_DebugLocker.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void XDEBUG(const char *fmt, ...)
{
	if (m_nDebugLevel != 0)
		return;

	va_list	ap;
	int		nLength;
    char    pszMessage[4096];

    memset(pszMessage, 0, sizeof(pszMessage));
	va_start(ap, fmt);
	vsprintf(pszMessage, fmt, ap);
	va_end(ap);

	nLength = strlen(pszMessage);

#ifndef _WIN32
	if (m_pDebugServer == NULL)
    {
#endif
	    m_DebugLocker.Lock();
		fprintf(m_pDebugFile, "%s", pszMessage);
	    m_DebugLocker.Unlock();
#ifdef _WIN32
        fflush(m_pDebugFile);
#endif
#ifndef _WIN32
    }
	else m_pDebugServer->Broadcast(pszMessage, nLength);
#endif
}

void _dumpStream(const char *pszBuffer, int nLength, int bPrintAscii, FILE *fp, BOOL bBroadcast)
{
	char	szString[512];
	char	szPart[64];
    char    szChar[8];
    char    szSpace[128];
	BYTE	c;
    int     i, n;

	if (!pszBuffer || !nLength)
		return;

    memset(szString, 0, sizeof(szString));
    memset(szPart, 0, sizeof(szPart));

	m_DebugLocker.Lock();
    for(i=0,n=0; i<nLength; i++)
    {
        c = pszBuffer[i];
        sprintf(szChar, "%02X ", c);
        szPart[n] = (c < ' ') || (c > 'z') ? '.' : c;
        n++;
        strcat(szString, szChar);
        if (i && ((i+1) % 25) == 0)
        {
            if (bPrintAscii)
            {
                szPart[n] = 0;
                strcat(szString, " ");
                strcat(szString, szPart);
                strcat(szString, "\r\n");
            }
            else
            {
                strcat(szString, "\r\n");
            }
            n = 0;
#ifndef _WIN32
            if (m_pDebugServer == NULL || bBroadcast == FALSE)
            {
#endif
                if(fp != NULL)
                {
                    fprintf(fp, " %s", szString);
                }
#ifndef _WIN32
            }
            else
            {
                m_pDebugServer->Broadcast(szString, strlen(szString));
            }
#endif
            szString[0] = 0;
        }
    }

    if(n) // 출력할 것이 남아 있을 때
    {

        if (bPrintAscii)
        {
            szPart[n] = 0;
            memset(szSpace, 0, sizeof(szSpace));
            for(i=0; i<(25-n); i++)
            {
                strcat(szSpace,"   ");
            }
        }
#ifndef _WIN32
        if (m_pDebugServer == NULL || bBroadcast == FALSE)
        {
#endif
            if (bPrintAscii)
            {
                if(fp != NULL)
                {
                    fprintf(fp, " %s%s %s\r\n", szString, szSpace, szPart);
                }
            }
            else 
            {
                if(fp != NULL)
                {
                    fprintf(fp, " %s\r\n", szString);
                }
            }
#ifndef _WIN32
        }
        else
        {
            if (bPrintAscii)
            {
                szPart[n] = 0;
                m_pDebugServer->Broadcast(" ", 1);
                m_pDebugServer->Broadcast(szString, strlen(szString));
                m_pDebugServer->Broadcast(szSpace, strlen(szSpace));
                m_pDebugServer->Broadcast(" ", 1);
                m_pDebugServer->Broadcast(szPart, strlen(szPart));
                m_pDebugServer->Broadcast("\r\n", 2);
            }
            else
            {
                m_pDebugServer->Broadcast(" ", 1);
                m_pDebugServer->Broadcast(szString, strlen(szString));
                m_pDebugServer->Broadcast("\r\n", 2);
            }
        }
#endif
    }
    m_DebugLocker.Unlock();
}

/** XDUMP가 기존에 크기 제한없이 출력을 허용하던 것을 크기 제한이 있는 방식으로
  * 변경한다.
  */
void XDUMP(const char *pszBuffer, int nLength, int bPrintAscii, BOOL bDirect)
{
	if (!bDirect && (m_nDebugLevel != 0))
		return;

    _dumpStream(pszBuffer, nLength, bPrintAscii, m_pDebugFile, TRUE);
}

/** Filename을 지정하면 Dump해준다.
  * 내부적으로는 FDUMP()를 다시 호출한다.
  * File에 내용을 Write 할 때 DebugLock이 동작한다.
  *
  * @param szFileName 대상 File Name
  * @param pszBuffer Dump할 내용
  * @param nLength Dump할 길이
  * @param bPrintAscii Ascii 형태로 추가 정보 출력 여부
  * @param bNewLine Dump 후에 NewLine을 출력한다
  *
  */
void FILEDUMP(const char *szFileName, char *pszBuffer, int nLength, BOOL bPrintAscii, BOOL bNewLine)
{
	FILE		*fp;
	time_t		ltime=0;
	struct tm	*today;
	char		szPath[256];

    if(!szFileName) return;
 
	time(&ltime);
	today = localtime(&ltime);
	sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, szFileName,
			today->tm_year+1900, today->tm_mon+1, today->tm_mday);

	fp = fopen(szPath, "a+b");
	if (fp != NULL)
	{	
        FDUMP(fp, pszBuffer, nLength, bPrintAscii);
        if(bNewLine) fprintf(fp, "\r\n");
        fclose(fp);
    }
}

/** File Pointer를 지정하면 Dump해준다.
  * 
  * File Dump에서 호출되기 대문에 Locker를 삭제한다.
  *
  * @param pFile 대상 File Pointer
  * @param pszBuffer Dump할 내용
  * @param nLength Dump할 길이
  * @param bPrintAscii Ascii 형태로 추가 정보 출력 여부
  *
  */
void FDUMP(FILE *pFile, char *pszBuffer, int nLength, BOOL bPrintAscii)
{
	if (!pFile || !pszBuffer || !nLength)
		return;

    _dumpStream(pszBuffer, nLength, bPrintAscii, pFile, FALSE);
}
