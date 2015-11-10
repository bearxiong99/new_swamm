#include <unistd.h>
#ifndef _WIN32
#include <sys/wait.h>
#endif
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include "common.h"
#include "mcudef.h"
#include "CommonUtil.h"
#include "CLIService.h"
#include "CLIUtility.h"
#include "CLIInterface.h"
#include "ShellCommand.h"



extern	CLIHANDLER  *m_pCommandHandlers;
extern  CLIPARAMCHECKER m_CliParameters[];
char	m_szUpdateFileName[128] = "";

#define DEBUG

void CLIAPI_Startup(CLICONSTRUCT *pConstruct, int argc, char **argv)
{
    CCLIService     theService(pConstruct);

    theService.ParseParameters(argc, argv);
    theService.Startup();
    theService.Shutdown();
}

void CLIAPI_Shutdown()
{
}

int	CLIAPI_GetRunLevel()
{
	return m_pCLIService->GetRunLevel();
}

void CLIAPI_Exit()
{
#ifndef _WIN32
	m_pSerialProtocol->RestoreSerialSetting();
#endif
	exit(0);
}

void CLIAPI_DisplayHelp(CLISESSION *pSession, char *pszCommand)
{
	display_help(pSession, m_pCommandHandlers, 0, FALSE);
}

void CLIAPI_DisplayHistory(CLISESSION *pSession)
{
	int		i;

	for(i=0; i<pSession->nHistoryCount; i++)
	{
		if (pSession->pszHistory[i] && *pSession->pszHistory[i])
			OUTTEXT(pSession, "%2d: %s\xd\xa", i+1, pSession->pszHistory[i]);
	}	
}

BOOL CLIAPI_Command(CLISESSION *pSession, IF4Invoke *pInvoke, const char *pszCommand)
{
	TIMETICK  tmStart, tmNow;
	double	fElapse;
	int		nElapse, nError;
	BYTE	nAttr;

#ifdef DEBUG
	OUTTEXT(pSession, "\nCOMMAND(%s:%0d): CMD=%s, PARAM-CNT=%0d.\xd\xa",
						pInvoke->szAddress, pInvoke->port,
						pszCommand, pInvoke->nParamCount);
#endif
    GetTimeTick(&tmStart);
	nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_MCUIPC | IF4_CMDATTR_RESPONSE;
	nError = IF4API_Command(pInvoke, pszCommand, nAttr);
    GetTimeTick(&tmNow);
	nElapse = GetTimeInterval(&tmStart, &tmNow);
	fElapse = (double)nElapse / (double)1000;

#ifdef DEBUG
	OUTTEXT(pSession, "RESPONSE: ERRCODE=%0d, RESULT-CNT=%0d, Elapse %.3f ms.\xd\xa",
						pInvoke->nError, pInvoke->nResultCount, fElapse);
#endif

	if (nError != IF4ERR_NOERROR)
	{
		OUTTEXT(pSession, "ERROR(%0d): %s\xd\xa", nError, IF4API_GetErrorMessage(nError));
		return FALSE;
	}
	return TRUE;
}

BOOL CLIAPI_CommandTest(CLISESSION *pSession, IF4Invoke *pInvoke, char *pszCommand)
{
	TIMETICK  tmStart, tmNow;
	double	fElapse;
	int		nElapse, nError;
	BYTE	nAttr;

	OUTTEXT(pSession, "\nCOMMAND(%s:%0d): CMD=%s, PARAM-CNT=%0d.\xd\xa",
						pInvoke->szAddress, pInvoke->port,
						pszCommand, pInvoke->nParamCount);

    GetTimeTick(&tmStart);
	nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_MCUIPC | IF4_CMDATTR_RESPONSE;
	nError = IF4API_Command(pInvoke, pszCommand, nAttr);
    GetTimeTick(&tmNow);
	nElapse = GetTimeInterval(&tmStart, &tmNow);
	fElapse = (double)nElapse / (double)1000;

	OUTTEXT(pSession, "RESPONSE: ERRCODE=%0d, RESULT-CNT=%0d, Elapse %.3f ms.\xd\xa",
						pInvoke->nError, pInvoke->nResultCount, fElapse);

	if (nError != IF4ERR_NOERROR)
	{
		OUTTEXT(pSession, "ERROR(%0d): %s\xd\xa", nError, IF4API_GetErrorMessage(nError));
		return FALSE;
	}
	return TRUE;
}

int CLIAPI_ShowCommandLog(CLISESSION *pSession, char *pDate)
{
	if (!m_pCLIService->ShowLog(pSession, pDate))
		return -1;	
	return 0;
}

int CLIAPI_ClearCommandLog(CLISESSION *pSession)
{
	m_pCLIService->ClearLog();
	return 0;
}

/** WIN32에서는 지원하지 않는다.
  */
BOOL CLIAPI_FirmwareDownload(CLISESSION *pSession)
{
#ifndef _WIN32
    WORKSESSION *pWorker;
	char	szPath[100] = "";
    int     pid, child, status=0;

    pid = Spawn(child, 1, "/tmp");
    if (pid > 0)
    {
		if (pSession->nType == CLITYPE_TELNET)
		{
    		pWorker = (WORKSESSION *)pSession->pSession;
            dup2(pWorker->sSocket, STDIN_FILENO);
            dup2(pWorker->sSocket, STDOUT_FILENO);
            dup2(pWorker->sSocket, STDERR_FILENO);
		}
        execl("/usr/bin/rz", "rz", "-b", "-Z", "-y", NULL);
        exit(0);
    }

    waitpid(child, &status, 0);
 
    if (WIFEXITED(status)==0)
    {
        OUTTEXT(pSession, "\r\nDownload fail. [%d]\r\n", WEXITSTATUS(status));
        return FALSE;
    }

	if (!GetFirmwareFileName("/tmp", szPath))
	{
	    OUTTEXT(pSession, "File:%s\xd\xa", szPath);
	    OUTTEXT(pSession, "Update fail. (check filename or retry)\xd\xa");
		return FALSE;
	}
	
	// Change File Name
	strcpy(m_szUpdateFileName, szPath);
	rename(szPath, FIRMWARE_COMPRESS_FILE);
	return TRUE;
#else
    return FALSE;
#endif
}

/** WIN32에서는 지원하지 않는다 
 */
BOOL CLIAPI_PolicyDownload(CLISESSION *pSession)
{
#ifndef _WIN32
    WORKSESSION *pWorker;
    int     pid, child, status;

    pid = Spawn(child, 1, "/app/conf");
    if (pid > 0)
    {
		if (pSession->nType == CLITYPE_TELNET)
		{
    		pWorker = (WORKSESSION *)pSession->pSession;
            dup2(pWorker->sSocket, STDIN_FILENO);
            dup2(pWorker->sSocket, STDOUT_FILENO);
            dup2(pWorker->sSocket, STDERR_FILENO);
		}
        execl("/usr/bin/rz", "rz", "-b", "-Z", "-y", NULL);
        exit(0);
    }

    while(waitpid(child, &status, WNOHANG) == 0)
        usleep(1000000);
	return TRUE;
#else
    return FALSE;
#endif
}

BOOL CLIAPI_Store(CLISESSION *pSession)
{
#ifndef _WIN32
    WORKSESSION *pWorker;
    int     pid, child, status;

    pid = Spawn(child, 1, "/app/sw");
    if (pid > 0)
    {
		if (pSession->nType == CLITYPE_TELNET)
		{
    		pWorker = (WORKSESSION *)pSession->pSession;
            dup2(pWorker->sSocket, STDIN_FILENO);
            dup2(pWorker->sSocket, STDOUT_FILENO);
            dup2(pWorker->sSocket, STDERR_FILENO);
		}
        execl("/usr/bin/rz", "rz", "-b", "-Z", "-y", NULL);
        exit(0);
    }

    while(waitpid(child, &status, WNOHANG) == 0)
        usleep(1000000);
	return TRUE;
#else
    return FALSE;
#endif
}

UINT CLIAPI_GetAgentPort()
{
	return m_pCLIService->GetAgentPort();
}

