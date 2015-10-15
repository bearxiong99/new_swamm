//////////////////////////////////////////////////////////////////////
//
//	DebugServer.cpp: implementation of the CDebugServer class.
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
#include <semaphore.h>
#include <signal.h>
#include <time.h>

#include "copyright.h"
#include "DebugServer.h"
#include "DebugUtil.h"

CDebugServer	*m_pDebugServer = NULL;

//////////////////////////////////////////////////////////////////////
// CDebugServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebugServer::CDebugServer()
{
	m_pDebugServer = this;
}

CDebugServer::~CDebugServer()
{
}

//////////////////////////////////////////////////////////////////////
// CDebugServer Message Handlers
//////////////////////////////////////////////////////////////////////

BOOL CDebugServer::OnEnterSession(WORKSESSION *pSession)
{
	SET_DEBUG_MODE(0);
	AddSendStream(pSession, "\xd\xa", -1);
	AddSendStream(pSession, "     #####    ##  ###    ###  ##  #######\xd\xa", -1);
	AddSendStream(pSession, "    ##   ##   ##  ####  ####  ##  ##    ##\xd\xa", -1);
	AddSendStream(pSession, "   ##     ##  ##  ## #### ##  ##  ##    ##\xd\xa", -1);
	AddSendStream(pSession, "   #########  ##  ##  ##  ##  ##  #######\xd\xa", -1);
	AddSendStream(pSession, "   ##     ##  ##  ##      ##  ##  ##   ##\xd\xa", -1);
	AddSendStream(pSession, "   ##     ##  ##  ##      ##  ##  ##    ##\xd\xa", -1);
	AddSendStream(pSession, "   ##     ##  ##  ##      ##  ##  ##      ##\xd\xa", -1);
	AddSendStream(pSession, "\xd\xa", -1);
	AddSendStream(pSession, "AIMIR MCU Debug Console.\xd\xa", -1);
	AddSendStream(pSession, COPYRIGHT "\xd\xa\xd\xa", -1);
	return TRUE;
}

void CDebugServer::OnLeaveSession(WORKSESSION *pSession)
{
	if (GetSessionCount() <= 1)
		SET_DEBUG_MODE(1);
}

BOOL CDebugServer::OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength)
{
	return TRUE;
}

void CDebugServer::OnErrorSession(WORKSESSION *pSession, int nType)
{
}

void CDebugServer::OnCloseSession(WORKSESSION *pSession)
{
}

BOOL CDebugServer::OnTimeoutSession(WORKSESSION *pSession)
{
	return TRUE;
}

