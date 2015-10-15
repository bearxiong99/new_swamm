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

#ifndef __TCP_MULTIPLEX_SERVER_H__
#define __TCP_MULTIPLEX_SERVER_H__

#include <pthread.h>
#include "typedef.h"
#include "TCPSocket.h"
#include "LinkedList.h"
#include "Locker.h"

//  [9/9/2010 DHKim]
#if !defined(__DISABLE_SSL__)
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#endif

class CTCPMultiplexServer : public CTCPSocket
{
// Construction/Destruction
public:
	CTCPMultiplexServer();
	virtual	~CTCPMultiplexServer();

// Attribute
public:
	virtual	BOOL IsStarted() const;
	virtual	BOOL IsSession();
	virtual	int GetMaxSession();
	virtual	int GetSessionCount();
	virtual int GetTimeout() const;
	virtual void SetTimeout(int nTimeout=-1);
	virtual BOOL GetSession(WORKSESSION *pSession);
	//  [10/7/2010 DHKim]
#if !defined(__DISABLE_SSL__)
	BOOL SSLServer_Initialize();
#endif

// Operations
public:
	virtual	BOOL Startup(int nPort, int nMaxSession, int nTimeout=-1);
	virtual	void Shutdown();

	void	Broadcast(const char *pszBuffer, int nLength);
	TID 	AddSendStream(WORKSESSION *pSession, const char *pszBuffer, int nLength);
	virtual int	SendToSession(WORKSESSION *pSession, const char *pszBuffer, int nLength=-1);
	virtual int	SendToSession(WORKSESSION *pSession, char c);

// Member Functions
protected:
	BOOL 	CreateDaemonSocket(int nPort, int nMaxSession);
	void 	CloseDaemonSocket();
	WORKSESSION *NewSession(SOCKET sSocket, LPSTR pszAddress);
	void 	DeleteSession(WORKSESSION *pSession);
	void 	DeleteAllSession();
	void 	RemoveAllStream(WORKSESSION *pSession);
	int 	SendStreamToSession(SOCKET sSocket, char *pszBuffer, int nLength);
	void 	SessionError(WORKSESSION *pSession, int nType);

// Override Functions
public:
	virtual	BOOL EnterDaemon();
	virtual	void LeaveDaemon();
	virtual	BOOL RunDaemon();
	virtual	BOOL SessionSender();

protected:
	virtual	WORKSESSION *AcceptSession();
	virtual	BOOL EnterSession(WORKSESSION *pSession);
	virtual	void LeaveSession(WORKSESSION *pSession);
	virtual	WORKSESSION *FindSession(SOCKET sSocket);
	virtual BOOL AddSessionRecvStream(WORKSESSION *pSession, char *pszBuffer, int nLength);
	virtual	TCPDATASTREAM *GetStream();

// Message Handlers
protected:
	virtual	BOOL OnEnterDaemon();
	virtual	void OnLeaveDaemon();

	virtual	void OnAcceptSession(WORKSESSION *pSession);
	virtual	BOOL OnEnterSession(WORKSESSION *pSession);
	virtual	void OnLeaveSession(WORKSESSION *pSession);
	virtual	void OnSendSession(WORKSESSION *pSession, const char* pszBuffer, int nLength);
	virtual	BOOL OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength);
	virtual	void OnErrorSession(WORKSESSION *pSession, int nType);
	virtual	void OnCloseSession(WORKSESSION *pSession);
	virtual	BOOL OnTimeoutSession(WORKSESSION *pSession);

// Work Thread
public:
	static	void *WatchThread(void *pParam);
	static	void *SendThread(void *pParam);

// Member Variable
private:
	BOOL							m_bStarted;
	BOOL							m_bExitPending;
	int								m_nMaxSession;
	int								m_nTimeout;
	pthread_t						m_watchThreadID;
	pthread_t						m_sendThreadID;
	sem_t							m_sendSem;
    
	//  [10/7/2010 DHKim]
#if !defined(__DISABLE_SSL__)
	SSL_CTX *                       s_ctx;
    SSL *                           s_ssl;
#endif

public:
	CLocker							m_SessionLocker;
	CLocker							m_StreamLocker;
	CLinkedList<WORKSESSION *>		m_SessionList;
	CLinkedList<TCPDATASTREAM *>	m_StreamList;
};

#endif	// __TCP_MULTIPLEX_SERVER_H__
