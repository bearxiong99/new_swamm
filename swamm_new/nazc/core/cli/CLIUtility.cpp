#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "common.h"
#include "CommonUtil.h"
#include "CLIService.h"
#include "CLIUtility.h"
#include "MemoryDebug.h"

extern  CLIHANDLER  	*m_pCommandHandlers;
extern  CLIPARAMCHECKER m_CliParameters[];
#if     defined(__PATCH_6_2084__)
extern  BOOL    m_bEnableHiddenCommand;
#endif

int SIGNED(unsigned char n)
{
	int		x;

	if (n & 0x80)
	{
		x = (int)(!n) + 1;
		return -x;
	}
	return n;
}

void MKPRINTDATE(char *pszTarget, char *pszSource)
{
    strcpy(pszTarget, "0000/00/00 00:00:00");
	if (!pszSource || !*pszSource)
		return;

    memcpy(pszTarget, pszSource, 4);
    memcpy(&pszTarget[5], &pszSource[4], 2);
    memcpy(&pszTarget[8], &pszSource[6], 2);
    memcpy(&pszTarget[11], &pszSource[8], 2);
    memcpy(&pszTarget[14], &pszSource[10], 2);
    memcpy(&pszTarget[17], &pszSource[12], 2);
}

void MKTIMESTR(char *pszTimeString)
{
    time_t  now;
    struct  tm  when;

    if (!pszTimeString)
        return;

    time(&now);
    when = *localtime(&now);

    sprintf(pszTimeString, "%04d%02d%02d%02d%02d%02d",
            when.tm_year + 1900,
            when.tm_mon + 1,
            when.tm_mday,
            when.tm_hour,
            when.tm_min,
            when.tm_sec);
}

void MKPRINTTIMESTR(char *pszTimeString, time_t *now)
{
    struct  tm  when;

    if (!pszTimeString)
        return;

	if (*now == 0)
		strcpy(pszTimeString, "0000/00/00 00:00:00");

    when = *localtime(now);
    sprintf(pszTimeString, "%04d/%02d/%02d %02d:%02d:%02d",
            when.tm_year + 1900,
            when.tm_mon + 1,
            when.tm_mday,
            when.tm_hour,
            when.tm_min,
            when.tm_sec);
}

void TIMESTAMP2STR(char *pszString, TIMESTAMP *pTime, BOOL bPrintable)
{
	if (!pszString)
		return;

	if (bPrintable)
	{
		sprintf(pszString, "%04d/%02d/%02d %02d:%02d:%02d",
						pTime->year, pTime->mon, pTime->day,
						pTime->hour, pTime->min, pTime->sec);
	}
	else
	{
		sprintf(pszString, "%04d%02d%02d%02d%02d%02d",
						pTime->year, pTime->mon, pTime->day,
						pTime->hour, pTime->min, pTime->sec);
	}
}

void OUTTEXT(CLISESSION *pSession, const char *fmt, ...)
{
    va_list ap;
    char    szMessage[2048];
    
    va_start(ap, fmt);
    vsprintf(szMessage, fmt, ap);
    va_end(ap);

	m_pCLIService->OutText(pSession, szMessage);
}

void PUT(CLISESSION *pSession, char *pszBuffer, int nLength)
{
	m_pCLIService->OutText(pSession, pszBuffer, nLength);
}

void PRINT(CLISESSION *pSession, char *pszBuffer)
{
	m_pCLIService->OutText(pSession, pszBuffer);
}

void do_not_support(CLISESSION *pSession)
{
	OUTTEXT(pSession, "Not supported operation.\xd\xa");
}

BOOL GenericInvoke(CLISESSION *pSession, CIF4Invoke *pInvoke)
{
/*
	GCPINVOKE	*pFiber;
	char	szMessage[256];
	int		nResult, nError;

	nError = pInvoke->Invoke();
	if (nError != GCPERR_NOERROR)
	{
		GCPAPI_GetErrorMessage(nError, szMessage);
		OUTTEXT(pSession, "ERROR(IPC%0d): %s\xd\xa", nError, szMessage);
		return FALSE;
	}

	pFiber = pInvoke->GetHandle();
	if (pFiber && pFiber->nResCount)
	{
		if (pFiber->pResult[pFiber->nResCount-1]->type == GCPDATA_TYPE_INT)
		{
			nResult = pFiber->pResult[pFiber->nResCount-1]->data.value;
			if (nResult != GCPRES_OK)
			{
				ReportError(pSession, 0, nResult);
				return FALSE;
			}
		}
	}
*/
	return TRUE;
}

void display_help(CLISESSION *pSession, CLIHANDLER *pList, int nDepts, BOOL bFirstOnly)
{
	char	szSpace[80];
	int		i;

	for(i=0; pList[i].pszCommand; i++)
	{
#if     defined(__PATCH_6_2084__)
        if(m_bEnableHiddenCommand && pList[i].bHidden) continue;
#else
        if(pList[i].bHidden) continue;
#endif
	    if (pList[i].nGroup < pSession->nGroup) continue;

		memset(szSpace, ' ', nDepts*3);
		szSpace[nDepts*3] = '\0';
		strcat(szSpace, pList[i].pszCommand);
		OUTTEXT(pSession, " %c%-22s %s\xd\xa", 
                pList[i].bHidden ? '*' : ' ',
                szSpace, pList[i].pszDescr);

		if (!bFirstOnly && pList[i].pNext != NULL)
			display_help(pSession, pList[i].pNext, nDepts+1);	
	}
}

BOOL GetFirmwareFileName(const char *pszPath, char *pszFileName)
{
    DIR     *dir_fdesc;
    dirent  *dp;
    char    *p;
    BOOL    bFind = FALSE;

    dir_fdesc = opendir(pszPath);
    if (!dir_fdesc)
        return FALSE;

    for(;(dp=readdir(dir_fdesc));)
    {
        p = strstr(dp->d_name, ".tar.gz");
		if (p == NULL)
			continue;

        sprintf(pszFileName, "%s/%s", pszPath, dp->d_name);
        bFind = TRUE;
        break;
    }
    closedir(dir_fdesc);
    return bFind;
}

void GetFileData(char *pszBuffer, char *pszFileName, int nLength)
{
	FILE	*fp;

	if (!pszBuffer)
		return;

	*pszBuffer = '\0';
	fp = fopen(pszFileName, "rb");
	if (fp == NULL)
		return;

	fread(pszBuffer, nLength, 1, fp);
	if ((pszBuffer[strlen(pszBuffer)-1] == '\xd') ||
		(pszBuffer[strlen(pszBuffer)-1] == '\xa'))
		pszBuffer[strlen(pszBuffer)-1] = '\0';

	fclose(fp);	
}

void GetFileDataEx(char *pszBuffer, char *pszFileName, int nLength)
{
	char	*pSpace;
	int		i, nSeek, nSize;

	pszBuffer[0] = '\0';
	pSpace = (char *)MALLOC(nLength+1);
	if (pSpace != NULL)
	{
		memset(pSpace, 0, nLength);
		GetFileData(pSpace, pszFileName, nLength);
		nSize = strlen(pSpace);

		for(i=0, nSeek=0; i<nSize; i++)
		{
			if ((pSpace[i] == 0xd) || (pSpace[i] == 0xa))
			{
				pszBuffer[nSeek] = 0xd;
				nSeek++;
				pszBuffer[nSeek] = 0xa;
			}
			else
			{
				pszBuffer[nSeek] = pSpace[i];
			}
			nSeek++;
		}
		FREE(pSpace);
	}
}

void SetFileData(char *pszFileName, char *pszBuffer, int nLength)
{
	FILE	*fp;

	if (!pszBuffer || !pszFileName)
		return;

	fp = fopen(pszFileName, "wb");
	if (fp == NULL)
		return;

	fwrite(pszBuffer, nLength, 1, fp);
	fclose(fp);	
}

BOOL Confirm(CLISESSION *pSession, const char *pszMessage)
{
	WORKSESSION *pWorker;
	struct  timeval tmStart, tmNow;
	char	szConfirm[10] = "n";
	int		nElapse, len = 0;

	OUTTEXT(pSession, pszMessage);
	pWorker = (WORKSESSION *)pSession->pSession;
	memset(szConfirm, 0, 10);

    gettimeofday((struct timeval *)&tmStart, NULL);
	for(;len<=0;)
	{
		len = read(pWorker->sSocket, szConfirm, 1);
		if (len <= 0)
		{
    		gettimeofday((struct timeval *)&tmNow, NULL);
			nElapse = GetTimevalInterval(&tmStart, &tmNow);
			if (nElapse > 5000)
				break;
		}
	}

	if (len > 0)
		 OUTTEXT(pSession, szConfirm);
	else OUTTEXT(pSession, "n");
	OUTTEXT(pSession, "\xd\xa");

	if ((szConfirm[0] == 'y') || (szConfirm[0] == 'Y'))
		return TRUE;

	if (len == 0)
	{
		OUTTEXT(pSession, "Canceled.\xd\a");
		OUTTEXT(pSession, "\xd\xa");
	}
	return FALSE;
}

int More(CLISESSION *pSession, int nCount, int nCurrent)
{
	WORKSESSION *pWorker;
	struct  timeval tmStart, tmNow;
    struct  timeval tv_timeo = {5, 0};
	char	szConfirm[10] = "n";
	int		nElapse, len = 0;

	if (nCount == 0)
	     OUTTEXT(pSession, "\r Press anykey to continue (y/n/a): ");
	else OUTTEXT(pSession, "\r Press anykey to continue [%0d/%0d] (y/n/a): ", nCurrent, nCount);
	pWorker = (WORKSESSION *)pSession->pSession;
	memset(szConfirm, 0, 10);

    gettimeofday((struct timeval *)&tmStart, NULL);
	for(;len<=0;)
	{
        // Issue_2098 : socket에 option을 설정하여 5초 이상이 될 경우 Timeout을 한다.
        setsockopt(pWorker->sSocket, SOL_SOCKET, SO_RCVTIMEO, &tv_timeo, sizeof(tv_timeo));
		len = read(pWorker->sSocket, szConfirm, 8);
		if (len <= 0)
		{
    		gettimeofday((struct timeval *)&tmNow, NULL);
			nElapse = GetTimevalInterval(&tmStart, &tmNow);
			if (nElapse > 5000)
				break;
		}
	}

	OUTTEXT(pSession, "\r                                                                 \r");
	if (len > 0)
	{
		if ((szConfirm[0] == 'n') || (szConfirm[0] == 'N'))
			return 0;

		if ((szConfirm[0] == 'a') || (szConfirm[0] == 'A'))
			return 2;
	}

	return 1;
}

void AddString(char *pszString, char *pszAdd)
{
	if (!pszString || !pszAdd || !*pszAdd)
		return;

	if (*pszString) strcat(pszString, "-");
	strcat(pszString, pszAdd);
}

void ReportError(CLISESSION *pSession, int nError, int nResult)
{
	OUTTEXT(pSession, "RESULT(%0d): ", nResult);
}

void SENSOREUI64STR(char *pszGUID)
{
    char    szBuffer[64];
    int     i;
    
    if (!pszGUID || !*pszGUID)
        return;
        
    strcpy(szBuffer, pszGUID);
    for(i=0; i<16; i+=2)
    {
        pszGUID[i]   = szBuffer[14-i];
        pszGUID[i+1] = szBuffer[15-i];
    }
}

void OUTDUMP(CLISESSION *pSession, char *pszBuffer, int nLength, BOOL bPrintAscii)
{
	char	*pszString;
	char	szPart[50];
    char    szChar[10];
	BYTE	c;
    int     i, len, n;

	if (!pszBuffer || !nLength)
		return;

	len = (nLength*4) + (nLength/25)*4 + 5; 
	pszString = (char *)MALLOC(len);
	if (pszString)
	{
		*pszString = '\0';
		memset(szPart, 0, 50);
   		for(i=0,n=0; i<nLength; i++)
  	  	{
	        c = pszBuffer[i];
  	      	sprintf(szChar, "%02X ", c);
			szPart[n] = (c < ' ') || (c > 'z') ? '.' : c;
			n++;
        	strcat(pszString, szChar);
			if (i && ((i+1) % 25) == 0)
			{
                if(bPrintAscii)
                {
				    szPart[n] = 0;
				    strcat(pszString, " ");
				    strcat(pszString, szPart);
                }
				strcat(pszString, "\xd\xa ");
				n = 0;
			}
    	}

		szPart[n] = 0;
   		OUTTEXT(pSession, " %s", pszString);
        if(bPrintAscii)
        {
		    for(i=0; i<(25-n); i++)
			    OUTTEXT(pSession, "   ");
		    OUTTEXT(pSession, " %s\xd\xa", szPart);
        }
        else
        {
		    OUTTEXT(pSession, "\xd\xa");
        }
		FREE(pszString);
	}
}

