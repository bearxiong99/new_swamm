//////////////////////////////////////////////////////////////////////
//
//	TCPServer.h: interface for the CTCPServer class.
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

#ifndef __DEBUG_SERVER_H__
#define __DEBUG_SERVER_H__

#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include "TCPMultiplexServer.h"

class CDebugServer : public CTCPMultiplexServer
{
// Construction/Destruction
public:
	CDebugServer();
	virtual	~CDebugServer();

// Message Handlers
protected:
	virtual	BOOL OnEnterSession(WORKSESSION *pSession);
	virtual	void OnLeaveSession(WORKSESSION *pSession);
	virtual	BOOL OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength);
	virtual	void OnErrorSession(WORKSESSION *pSession, int nType);
	virtual	void OnCloseSession(WORKSESSION *pSession);
	virtual	BOOL OnTimeoutSession(WORKSESSION *pSession);
};

extern CDebugServer	*m_pDebugServer;

#endif	// __DEBUG_SERVER_H__
