
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#include "typedef.h"

/** 기능 중 일부분을 옮겨 온다.
  * lanucher는 shared library를 쓰지 않게 작성하기 위해
  * 추가로 file을 옮겨 온다.
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

void UpdateLauncherLogFile(const char * szLogDir, const char * szLogFile, int nLimitSize, BOOL bTime, const char *fmt, ...)
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

	//m_DebugLocker.Lock();
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
	//m_DebugLocker.Unlock();
}

