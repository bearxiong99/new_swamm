//////////////////////////////////////////////////////////////////////
//
//	TCPClient.h: interface for the CTCPClient class.
//
//	This file is a part of the AIMIR-.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <semaphore.h>
#include <pthread.h>
#include "TCPSocket.h"
#include "LinkedList.h"
#include "Locker.h"

class CTCPClient : public CTCPSocket
{
// Construction/Destruction
public:
	CTCPClient();
	virtual ~CTCPClient();

// Attribute
public:
	BOOL IsConnected() const;
	BOOL SSLClient_Initialize();		//  [10/7/2010 DHKim]

// Operations
public:
	BOOL Connect(const char* pszAddress, int nPort, BOOL bAsync=FALSE);
	void Disconnect();

// Member Functions
public:
	TID AddSendStream(LPSTR pszBuffer, int nLength=-1);

protected:
	void FreeStream(TCPDATASTREAM *pStream);
    BOOL IsPendingSendQ();
	TCPDATASTREAM *GetStream();
	void RemoveAllSendStream();
	int SendStream(char *pszBuffer, int nLength);

// Socket Handler Functions
protected:
	BOOL ConnectHandler();
	void CloseHandler();
	void ConnectErrorHandler();
	BOOL SendHandler();
	BOOL ReceiveHandler();
	void ErrorHandler(int nType);

// Socket Message Handlers
public:
	virtual void OnConnect();
	virtual void OnDisconnect();
	virtual void OnConnectError();
	virtual void OnSend(LPSTR pszBuffer, int nLength);
	virtual BOOL OnReceive(LPSTR pszBuffer, int nLength);
	virtual void OnError(int nType);

// Thread Message Handlers
protected:
	void RunWatchThread();
	void RunSendThread();

// Static Functions
public:
	static	void *WatchThread(void *pParam);
	static	void *SendThread(void *pParam);

// Member Variable
protected:
	BOOL	m_bConnected;
	BOOL	m_bReadySend;
	BOOL	m_bReadyRecv;
	BOOL	m_bExitSendPending;
	BOOL	m_bExitRecvPending;
	TID		m_nTID;
	char	m_szBuffer[DEFAULT_SESSION_BUFFER];
	pthread_t						m_watchThreadID;
	pthread_t						m_sendThreadID;
	sem_t							m_sendSem;
	CLocker							m_SendLocker;
	CLinkedList<TCPDATASTREAM *>	m_SendList;

	//  [10/7/2010 DHKim]
	SSL_CTX* m_ctx;
	SSL*     m_ssl;

};

#endif // __TCP_CLIENT_H__
