//////////////////////////////////////////////////////////////////////
//
//  CLISerialProtocol.cpp: implementation of the CCLISerialProtocol class.
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
#include <time.h>

#include "common.h"
#include "CLIService.h"
#include "CLISerialProtocol.h"
#include "CLIUtility.h"

//////////////////////////////////////////////////////////////////////
// CCLISerialProtocol Data Definitions
//////////////////////////////////////////////////////////////////////

CCLISerialProtocol		*m_pSerialProtocol = NULL;

//////////////////////////////////////////////////////////////////////
// CCLISerialProtocol Class
//////////////////////////////////////////////////////////////////////

CCLISerialProtocol::CCLISerialProtocol()
{
	m_pSerialProtocol = this;
}

CCLISerialProtocol::~CCLISerialProtocol()
{
}

//////////////////////////////////////////////////////////////////////
// CCLISerialProtocol Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CCLISerialProtocol::Start()
{
	if (m_pSerialProtocol == NULL)
		return FALSE;

#ifndef _WIN32
	CSerialServer::Startup(m_pProtocol->pszDevice, m_pProtocol->nMaxSession, CLIDEFAULT_TIMEOUT, B115200);
#else
    Connect(m_pProtocol->pszDevice, CBR_115200, NOPARITY, 8, ONESTOPBIT);
#endif
	return TRUE;
}

void CCLISerialProtocol::Stop()
{
#ifndef _WIN32
	CSerialServer::Shutdown();
#else
    Disconnect();
#endif
}

BOOL CCLISerialProtocol::Write(CLISESSION *pSession, char *pszBuffer, int nLength)
{
	return WriteStream(pSession, pszBuffer, nLength);
}

//////////////////////////////////////////////////////////////////////
// Message Handlers
//////////////////////////////////////////////////////////////////////

BOOL CCLISerialProtocol::OnEnterSession(WORKSESSION *pSession)
{
	CLISESSION	*pTerminal;

//	printf("EnterSession = '%s'\n", pSession->szAddress);

	// Create New Session Wrapper
	pTerminal = NewTerminalSession((void *)pSession, CLITYPE_SERIAL);
	if (pTerminal == NULL)
		return FALSE;

	// Save Self Session
	pTerminal->pProtocol  = (void *)m_pSerialProtocol;
	pTerminal->pszAddress = pSession->szAddress;
	pSession->dwUserData  = (UINT_PTR)pTerminal;

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

void CCLISerialProtocol::OnLeaveSession(WORKSESSION *pSession)
{
	CLISESSION	*pTerminal;

//	printf("LeaveSession = '%s'\n", pSession->szAddress);

	// Load Self Session
	pTerminal = (CLISESSION *)pSession->dwUserData;
	if (pTerminal == NULL)
		return;

	// Callback Runtime
	if (m_pCLIService->m_pConstruct &&
		m_pCLIService->m_pConstruct->pfnOnLeaveSession)
		m_pCLIService->m_pConstruct->pfnOnLeaveSession(pTerminal);

	DeleteTerminalSession(pTerminal);
}

BOOL CCLISerialProtocol::OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength)
{
	time(&pSession->lLastInput);
	if (!InputStream((CLISESSION *)pSession->dwUserData, (unsigned char *)pszBuffer, nLength))
		RestartTerminalSession((CLISESSION *)pSession->dwUserData);

	return TRUE;
}

void CCLISerialProtocol::OnErrorSession(WORKSESSION *pSession, int nType)
{
//	printf("ErrorSession = '%s'\n", pSession->szAddress);
}

void CCLISerialProtocol::OnCloseSession(WORKSESSION *pSession)
{
//	printf("CloseSession = '%s'\n", pSession->szAddress);
}

BOOL CCLISerialProtocol::OnTimeoutSession(WORKSESSION *pSession)
{
	CLISESSION	*pCliSession;

	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
		return TRUE;

	pCliSession = (CLISESSION *)pSession->dwUserData;
	if (!pCliSession->bLogined)
	{
		time(&pSession->lLastInput);
		return TRUE;
	}

	time(&pSession->lLastInput);
	if (!pCliSession->bDebugMode)
		RestartTerminalSession(pCliSession);
	return TRUE;
}

BOOL CCLISerialProtocol::WriteChar(CLISESSION *pSession, unsigned char c)
{
#ifdef _WIN32
	return CSerialWriter::AddSendStream(c);
#else
	return SendToSession((WORKSESSION *)pSession->pSession, (char)c) == -1 ? FALSE : TRUE;
#endif
}

BOOL CCLISerialProtocol::WriteStream(CLISESSION *pSession, const char *pszBuffer, int nLength)
{
	nLength = nLength == -1 ? strlen(pszBuffer) : nLength;
#ifdef _WIN32
	return CSerialWriter::AddSendStream((LPSTR)pszBuffer, nLength);
#else
	return SendToSession((WORKSESSION *)pSession->pSession, pszBuffer, nLength) == -1 ? FALSE : TRUE;
#endif
}
