//////////////////////////////////////////////////////////////////////
//
//  CLIService.cpp: implementation of the CCLIService class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "mcudef.h"
#include "CLIService.h"
#include "CLISerialProtocol.h"
#include "CLITcpipProtocol.h"
#include "CLIUtility.h"
#include "DebugUtil.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// CCLIService Class
//////////////////////////////////////////////////////////////////////

CCLIService		*m_pCLIService 			= NULL;
char			*m_pMcuVersionString 	= NULL;

#if !defined(__DISABLE_SSL__)
extern BYTE		m_nSecurityState;
#endif

//////////////////////////////////////////////////////////////////////
// CCLIService Class
//////////////////////////////////////////////////////////////////////

CCLIService::CCLIService(CLICONSTRUCT *pConstruct)
{
	m_pCLIService		= this;
	m_pConstruct		= pConstruct;
	m_pMcuVersionString = pConstruct ? pConstruct->pszVersion : NULL;
	m_nRunLevel			= RUNLEVEL_NORMAL;
    m_nAgentPort        = 0;
	memset(&m_Setting, 0, sizeof(CLISETTING));
}

CCLIService::~CCLIService()
{
}

//////////////////////////////////////////////////////////////////////
// CCLIService Attribute
//////////////////////////////////////////////////////////////////////

BOOL CCLIService::IsExpert()
{
	return m_Setting.bEnableExpert;
}

int CCLIService::GetRunLevel() const
{
	return m_nRunLevel;
}

BOOL CCLIService::SetExpert(BOOL bEnable)
{
	m_ConfigLocker.Lock();
	m_Setting.bEnableExpert = bEnable;
	m_ConfigLocker.Unlock();

	return SaveSetting();
}

//////////////////////////////////////////////////////////////////////
// CCLIService Functions
//////////////////////////////////////////////////////////////////////

BOOL CCLIService::ParseParameters(int argc, char **argv)
{
#ifndef _WIN32
	int		opt;

	if (m_pConstruct && m_pConstruct->pfnOnParseParameter)
		m_pConstruct->pfnOnParseParameter(argc, argv);

    while((opt = getopt(argc, argv, "D")) != -1) 
    {
        switch(opt) 
        { 
            case 'D':
				 m_nRunLevel = RUNLEVEL_DEBUG;
                 break; 
        }
    } 

	return TRUE;
#else
    return FALSE;
#endif
}

BOOL CCLIService::OnStarted()
{
	int		nResult;
	VAROBJECT 	*pObject;
    int     i;

	// Loading CLI Configuration
	if (!LoadSetting())
	{
		m_Setting.bEnableExpert = FALSE;
		SaveSetting();
	}

	// Loading User Information
	m_Setting.bEnableExpert = FALSE;

	VARAPI_Initialize(VARCONF_FILENAME, (VAROBJECT *)m_pConstruct->pObject, FALSE);

    // Local Port에 대한 설정 값을 얻어온다
	pObject = VARAPI_GetObjectByName("sysLocalPort");
	if (pObject != NULL) 
    {
        m_nAgentPort = pObject->var.stream.u32;
    }

    if(m_nAgentPort == 0)
    {
        m_nAgentPort = 8000; // Default Port
    }

#if !defined(__DISABLE_SSL__)
    // SSL 설정 정보 조회
	pObject = VARAPI_GetObjectByName("sysSecurityState");
	if (pObject != NULL) 
    {
        m_nSecurityState = pObject->var.stream.u8;
    }
#endif
    
    // initialize AIMIR-GCP
    nResult = IF4API_Initialize(m_nAgentPort, NULL);
    if (nResult != IF4ERR_NOERROR)
        return FALSE;

	// 작업 처리 Worker를 생성한다.
	for(i=0; i<CLI_WORKER_COUNT; i++)
		m_Worker[i].Initialize();

	if (m_bEnableSerial)
	{
		// Add Serial Protocol
		AddProtocol("serial", 
				CLIPROTOCOL_SERIAL, 
				m_szSerialDevice,
				0,
				CLIMAX_SERIAL_SESSION, 
				m_nIdleTimeout,
				(void *)m_pSerialProtocol);
	}

	if (GetRunLevel() != RUNLEVEL_DEBUG)
	{
		if (m_bEnableTelnet)
		{
			// Add Telnet Protocol
			AddProtocol("telnet", 
				CLIPROTOCOL_TCP, 
				NULL, 
				m_nTelnetPort, 
				m_nTelnetMaxSession-CLIMAX_SERIAL_SESSION, 
				m_nIdleTimeout, 
				(void *)m_pTcpipProtocol);
		}
	}

	StartProtocols();

	if (m_pConstruct && m_pConstruct->pfnOnStartup)
		m_pConstruct->pfnOnStartup();
	return TRUE;
}

void CCLIService::OnShutdown()
{
	int		i;

	if (m_pConstruct && m_pConstruct->pfnOnShutdown)
		m_pConstruct->pfnOnShutdown();

	// Stop Service
	StopProtocols();

	// Remove All Protocols
	RemoveAllProtocols();

	// Worker를 모두 종료한다.
	for(i=0; i<CLI_WORKER_COUNT; i++)
		m_Worker[i].Destroy();

	IF4API_Destroy();
    VARAPI_Destroy();
}

CWorker *CCLIService::GetWorker()
{
	CWorker	*pWorker = NULL;
	int		i;

	for(i=0; i<CLI_WORKER_COUNT; i++)
	{
		if (!m_Worker[i].IsActive())
		{
			pWorker = &m_Worker[i];
			break;
		}
	}
	return pWorker;
}

CWorker *CCLIService::Execute(PFNWORKERPROC pfnFunc, void *pThis, CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory)
{
	CWorker	*pWorker = NULL;
	int		nRetry;

	m_WorkerLocker.Lock();
	for(nRetry=0; nRetry<1000; nRetry++)
	{
		pWorker = GetWorker();
		if (pWorker != NULL)
		{
			pSession->pWorker = (void *)pWorker;
			pWorker->Execute(pThis, pfnFunc, pSession, bComplete, bAddHistory);
			break;
		}
		USLEEP(100000);
	}
	m_WorkerLocker.Unlock();	
	return pWorker;
}

BOOL CCLIService::WaittingForIPC(int nWaitTime)
{
	time_t		startup, now;
	BOOL		bState = FALSE, bPassed=FALSE;
	char		szBuffer[32];

	time(&startup);
	write(STDOUT_FILENO, "Loading", 7);
	for(;;)
	{
		time(&now);
		if ((now-startup) > nWaitTime)
			break;

	 	sprintf(szBuffer, "\rLoading %s%2d %s", 
							bPassed ? "IPC " : "",
							(int)(nWaitTime-(now-startup)),
							bState ? "-" : "|"); 
		
		write(STDOUT_FILENO, szBuffer, strlen(szBuffer));
		bState = !bState;

		USLEEP(500000);
	}

	write(STDOUT_FILENO, "\x8Time over!\xd\xa", 13);
	return FALSE;
}

BOOL CCLIService::LoadSetting()
{
	FILE	*fp;

	memset(&m_Setting, 0, sizeof(CLISETTING));
	m_ConfigLocker.Lock();
	fp = fopen("/app/conf/clirc", "rb");
	if (fp != NULL)
	{
		fread(&m_Setting, sizeof(CLISETTING), 1, fp);
		fclose(fp);
	}
	m_ConfigLocker.Unlock();

	CCLIOption::LoadOption();
	return TRUE;
}

BOOL CCLIService::SaveSetting()
{
	FILE	*fp;

	m_ConfigLocker.Lock();
	fp = fopen("/app/conf/clirc", "w+b");
	if (fp != NULL)
	{
		fwrite(&m_Setting, sizeof(CLISETTING), 1, fp);
		fclose(fp);
	}
	m_ConfigLocker.Unlock();

	return (fp == NULL) ? FALSE : TRUE;
}

BOOL CCLIService::DoLogin(CLISESSION *pSession, char *pszID, char *pszPassword)
{
	if (!m_pConstruct || !m_pConstruct->pfnDoLogin)
	{
		OUTTEXT(pSession, "Do not support login.\xd\xa");	
		return FALSE;
	}

	return m_pConstruct->pfnDoLogin(pSession, pszID, pszPassword);
}

void CCLIService::AddLog(char *pszUser, char *pszString)
{
#ifdef __SUPPORT_CLI_LOG__
	char	szTime[25];
	FILE	*fp;

	MKTIMESTR(szTime);
	m_LogLocker.Lock();
	fp = fopen(m_szLogPath, "a+b");
	if (fp != NULL)
	{
		sprintf(m_szMessage, "%s|%s|%s\n", szTime, pszUser, pszString);
		fwrite(m_szMessage, strlen(m_szMessage), 1, fp);
		fclose(fp);
	}
	m_LogLocker.Unlock();
#endif
}

BOOL CCLIService::ShowLog(CLISESSION *pSession, char *pDate)
{
	FILE	*fp;
	char	szBuffer[512], szTime[25];
	char	*p1, *p2, *p3;
	int		nLength;
	int		nMode = 0;

	if (strcmp(pDate, "all") == 0)
		nMode = 0;
	else nMode = 1;

	m_LogLocker.Lock();
	fp = fopen(m_szLogPath, "a+b");
	if (fp != NULL)
	{
		OUTTEXT(pSession, "==============================================================================\xd\xa");
		OUTTEXT(pSession, " DATE                USER       COMMAND\xd\xa");
		OUTTEXT(pSession, "==============================================================================\xd\xa");

		while(fgets(szBuffer, 511, fp))
		{
			nLength = strlen(szBuffer);
			if (szBuffer[nLength-1] == '\n')
				szBuffer[nLength-1] = '\0';

			p1 = strtok(szBuffer, "|");
			p2 = strtok(NULL, "|");
			p3 = strtok(NULL, "|");
			if ((nMode == 1) && (strncmp(p1, pDate, 8) != 0))
				continue;

			MKPRINTDATE(szTime, p1);
			OUTTEXT(pSession, " %s %-10s %s\xd\xa", szTime, p2, p3);	
		}
		fclose(fp);

		OUTTEXT(pSession, "==============================================================================\xd\xa");
	}
	else
	{
		OUTTEXT(pSession, "No command history.\xd\xa");	
	}
	m_LogLocker.Unlock();
	return TRUE;
}

BOOL CCLIService::ClearLog()
{
	m_LogLocker.Lock();
	remove(m_szLogPath);
	m_LogLocker.Unlock();
	return TRUE;
}

UINT CCLIService::GetAgentPort()
{
    return m_nAgentPort;
}

