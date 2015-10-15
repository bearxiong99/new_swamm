//////////////////////////////////////////////////////////////////////
//
//	TCPSocket.h: interface for the CTCPSocket class.
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

#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include "typedef.h"

//  [9/14/2010 DHKim]
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET					-1
#endif

#define DEFAULT_MAX_SESSION				1000
#define DAEMON_STACK_SIZE				4096
#define SESSION_STACK_SIZE				4096
#define DEFAULT_SESSION_BUFFER			4096
#define DEFAULT_CONNECT_TIMEOUT			10000
#define DEFAULT_KEEPALIVE_TIME			INFINITE
#define DEFAULT_SVR_SHUTDOWN_TIMEOUT	10000	
#define DEFAULT_SESSION_STOP_TIMEOUT	10000

#define TCPIO_TYPE_SEND					0
#define TCPIO_TYPE_RECV					1

#ifndef _WIN32
typedef int								SOCKET;
#endif
typedef int								TID;

class CTCPSocket  
{
// Construction/Destruction
public:
	CTCPSocket();
	virtual ~CTCPSocket();

// Attribute
public:
	void * GetHandle() const;
	virtual	int GetPort() const;
	virtual	void SetPort(int nPort);
	virtual const char* GetAddress() const;
	virtual	void SetAddress(const char* pszAddress);
	virtual void SetSocketParam(UINT_PTR dwParam);
	virtual UINT_PTR GetSocketParam();

// Operations
public:
	virtual	BOOL Create();
	virtual	void Close();
	virtual	void Shutdown();
	virtual	BOOL Bind(int nPort=-1);
	virtual	BOOL Listen(int nMaxSession=DEFAULT_MAX_SESSION);
	virtual	SOCKET Accept(LPSTR pszAddress=NULL);
	virtual	BOOL Connect(const char* pszAddress=NULL, int nPort=-1, BOOL bAsync=TRUE);

	virtual	int	Send(LPSTR pszBuffer, int nLength);
	virtual	int Receive(LPSTR pszBuffer, int nMaximum);

// Member Variable
protected:
	SOCKET	m_sSocket;
	WORD	m_nPort;
	int		m_nLastError;
	BOOL	m_bAsyncMode;
	UINT_PTR	m_dwSocketParam;
	char	m_szAddress[64];
};

#endif // __TCP_SOCKET_H__

