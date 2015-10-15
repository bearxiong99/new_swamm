//////////////////////////////////////////////////////////////////////
//
//  CLITcpipProtocol.cpp: implementation of the CCLITcpipProtocol class.
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

#ifndef _WIN32
#include <sys/ioctl.h>
#include <arpa/telnet.h>
#include <termios.h>
#endif
#include "common.h"
#include "CLIService.h"
#include "CLITcpipProtocol.h"
#include "CLIUtility.h"
#include "DebugUtil.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// CCLITcpipProtocol Data Definitions
//////////////////////////////////////////////////////////////////////

CCLITcpipProtocol		*m_pTcpipProtocol = NULL;

//////////////////////////////////////////////////////////////////////
// CCLITcpipProtocol Class
//////////////////////////////////////////////////////////////////////

CCLITcpipProtocol::CCLITcpipProtocol()
{
	m_pTcpipProtocol = this;
}

CCLITcpipProtocol::~CCLITcpipProtocol()
{
}

//////////////////////////////////////////////////////////////////////
// CCLITcpipProtocol Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CCLITcpipProtocol::Start()
{
	if (m_pTcpipProtocol == NULL)
		return FALSE;

	CTCPMultiplexServer::Startup(m_pProtocol->nPort, m_pProtocol->nMaxSession, CLIDEFAULT_TIMEOUT);
	return TRUE;
}

void CCLITcpipProtocol::Stop()
{
	CTCPMultiplexServer::Shutdown();
}

BOOL CCLITcpipProtocol::Write(CLISESSION *pSession, char *pszBuffer, int nLength)
{
	if (pSession->bBreak)
		return FALSE;

	return WriteStream(pSession, pszBuffer, nLength);
}

//////////////////////////////////////////////////////////////////////
// Message Handlers
//////////////////////////////////////////////////////////////////////

void CCLITcpipProtocol::OnAcceptSession(WORKSESSION *pSession)
{
	int		nOptValue;

    // turn on KEEPALIVE so if the client crashes, we'll know about it
	nOptValue	= 1;
	setsockopt(pSession->sSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&nOptValue, sizeof(nOptValue));
}

BOOL CCLITcpipProtocol::OnEnterSession(WORKSESSION *pSession)
{
	CLISESSION	*pTerminal;

//	printf("EnterSession = '%s'\n", pSession->szAddress);

	// Create New Session Wrapper
	pTerminal = NewTerminalSession((void *)pSession, CLITYPE_TELNET);
	if (pTerminal == NULL)
		return FALSE;

	// Save Self Session
	pTerminal->pProtocol  = (void *)m_pTcpipProtocol;
	pTerminal->pszAddress = pSession->szAddress;
	pSession->dwUserData  = (UINT_PTR)pTerminal;

//    dup2(pSession->sSocket, 0); //STDIN_FILENO);
//    dup2(pSession->sSocket, 1); //STDOUT_FILENO);
//    dup2(pSession->sSocket, 2); //STDERR_FILENO);

	// Change Local Echo Option
	SetupTerminal(pTerminal);

	// Callback Runtime
	if (m_pCLIService->m_pConstruct &&
		m_pCLIService->m_pConstruct->pfnOnEnterSession)
		m_pCLIService->m_pConstruct->pfnOnEnterSession(pTerminal);

	// Display Splash & Prompt
	DisplaySplash(pTerminal);
	return TRUE;
}

void CCLITcpipProtocol::OnLeaveSession(WORKSESSION *pSession)
{
	CLISESSION	*pTerminal;

//	printf("LeaveSession = '%s'\n", pSession->szAddress);

	// Load Self Session
	pTerminal = (CLISESSION *)pSession->dwUserData;
	if (pTerminal == NULL)
		return;
	
	// Callback Login
	if (pTerminal->bLogined)
	{
		if (m_pCLIService->m_pConstruct &&
			m_pCLIService->m_pConstruct->pfnOnLogout)
			m_pCLIService->m_pConstruct->pfnOnLogout(pTerminal);
	}

	// Callback Runtime
	if (m_pCLIService->m_pConstruct &&
		m_pCLIService->m_pConstruct->pfnOnLeaveSession)
		m_pCLIService->m_pConstruct->pfnOnLeaveSession(pTerminal);

	// Worker가 종료되지 않은 경우 대기한다.
	pTerminal->bBreak = TRUE;
	while(pTerminal->pWorker != NULL)
		USLEEP(100000);

	DeleteTerminalSession(pTerminal);	
	pSession->dwUserData = (UINT_PTR)0;
}

BOOL CCLITcpipProtocol::OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength)
{
//	printf("recv %0d bytes\xd\xa", nLength);
//	XDUMP(pszBuffer, nLength, TRUE);
	return InputStream((CLISESSION *)pSession->dwUserData, (const unsigned char *)pszBuffer, nLength);
}

void CCLITcpipProtocol::OnErrorSession(WORKSESSION *pSession, int nType)
{
//	printf("ErrorSession = '%s'\n", pSession->szAddress);
}

void CCLITcpipProtocol::OnCloseSession(WORKSESSION *pSession)
{
//	printf("CloseSession = '%s'\n", pSession->szAddress);
}

BOOL CCLITcpipProtocol::OnTimeoutSession(WORKSESSION *pSession)
{
	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
		return TRUE;

//	printf("TimeSession = '%s'\n", pSession->szAddress);
	return FALSE;
}

BOOL CCLITcpipProtocol::WriteChar(CLISESSION *pSession, unsigned char c)
{
	return SendToSession((WORKSESSION *)pSession->pSession, (char)c) == -1 ? FALSE : TRUE;
}

BOOL CCLITcpipProtocol::WriteStream(CLISESSION *pSession, const char *pszBuffer, int nLength)
{
	if (pSession->bBreak)
		return FALSE;

	nLength = nLength == -1 ? strlen(pszBuffer) : nLength;
	return SendToSession((WORKSESSION *)pSession->pSession, pszBuffer, nLength) == -1 ? FALSE : TRUE;
}
