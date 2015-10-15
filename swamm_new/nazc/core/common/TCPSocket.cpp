//////////////////////////////////////////////////////////////////////
//
//	TCPSocket.cpp: implementation of the CTCPSocket class.
//
//	This file is a part of the AIMIR-FCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#if     defined(_WIN32)  && !defined(_QT_BUILD) && !defined(__MINGW32__)
#include <afx.h>
#include <afxwin.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#ifndef  _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#else
  #include "stdafx.h"
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
#endif
#include <errno.h>
#ifdef _WIN32
  #include <winerror.h>
#endif
#include <signal.h>
#include <fcntl.h>

#include "TCPSocket.h"
#include "DebugUtil.h"

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT    0
#endif

//////////////////////////////////////////////////////////////////////////
//  [10/6/2010 DHKim]
//  extern data
//////////////////////////////////////////////////////////////////////////
#if !defined(__DISABLE_SSL__)
BYTE		m_nSecurityState = 0;
#endif
extern BOOL m_blocalcommand;
extern BOOL m_bSSLAccept;
//////////////////////////////////////////////////////////////////////
// CTCPSocket Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPSocket::CTCPSocket()
{
	m_sSocket		= INVALID_SOCKET;
	m_nPort			= 0;
	m_nLastError	= 0;
	m_dwSocketParam	= 0;
	m_bAsyncMode	= FALSE;
	*m_szAddress	= '\0';
}

CTCPSocket::~CTCPSocket()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
// CTCPSocket Attribute
//////////////////////////////////////////////////////////////////////

void * CTCPSocket::GetHandle() const
{
	return (void *)m_sSocket;
}

int CTCPSocket::GetPort() const
{
	return (int)m_nPort;
}

void CTCPSocket::SetPort(int nPort)
{
	m_nPort = (WORD)nPort;
}

const char* CTCPSocket::GetAddress() const
{
	return (const char*)m_szAddress;
}

void CTCPSocket::SetAddress(const char* pszAddress)
{
	strcpy(m_szAddress, pszAddress);
}

void CTCPSocket::SetSocketParam(UINT_PTR dwParam)
{
	m_dwSocketParam = dwParam;
}

UINT_PTR CTCPSocket::GetSocketParam()
{
	return m_dwSocketParam;
}

//////////////////////////////////////////////////////////////////////
// CTCPSocket Operations
//////////////////////////////////////////////////////////////////////

BOOL CTCPSocket::Create()
{
	struct	linger Linger; 
//	BOOL	bNoDelay = TRUE;

	// 소켓 초기화 생성
	m_sSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sSocket == INVALID_SOCKET)
	{
#ifdef _WIN32
                errno = WSAGetLastError();
#endif
		m_nLastError = errno;
		return FALSE;
	}

//	setsockopt(m_sSocket, IPPROTO_TCP, TCP_NODELAY, &bNoDelay, sizeof(bNoDelay));
#if !defined(__DISABLE_SSL__)
    if(m_nSecurityState == 0)
	{
#endif
		Linger.l_onoff	= 1; 
		Linger.l_linger = 0; 
#ifndef _WIN32
 		setsockopt(m_sSocket, SOL_SOCKET, SO_LINGER, &Linger, sizeof(Linger));
#else
        setsockopt(m_sSocket, SOL_SOCKET, SO_LINGER, (const char *)&Linger, sizeof(Linger));
#endif
#if !defined(__DISABLE_SSL__)
	}
#endif

	return TRUE;
}

void CTCPSocket::Close()
{
	if (m_sSocket == INVALID_SOCKET)
		return;

#ifdef _WIN32
        errno = WSAGetLastError();
#endif

	m_nLastError = errno;
#ifdef _WIN32
    closesocket(m_sSocket);
#else
	close(m_sSocket);
#endif
	m_sSocket = INVALID_SOCKET;
}

void CTCPSocket::Shutdown()
{
	if (m_sSocket == INVALID_SOCKET)
		return;

#ifdef _WIN32
    shutdown(m_sSocket, SD_BOTH);
#else
	shutdown(m_sSocket, SHUT_RDWR);
#endif
}

BOOL CTCPSocket::Bind(int nPort)
{
	struct	sockaddr_in		addr;
	int		addrlen = sizeof(struct sockaddr_in);
	int		nError;

	if (m_sSocket == INVALID_SOCKET)
		return FALSE;

	if (nPort != -1)
		m_nPort = (WORD)nPort;

//	XDEBUG("Port Number = %0d\n", m_nPort);

	memset(&addr, 0, addrlen);
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	addr.sin_port			= htons(m_nPort);

	nError = bind(m_sSocket, (struct sockaddr *)&addr, addrlen);
	if (nError < 0)
	{
		Close();
		return FALSE;
	}

	return TRUE;
}

BOOL CTCPSocket::Listen(int nMaxSession)
{
	int		nError;

	if (m_sSocket == INVALID_SOCKET)
		return FALSE;

	nError = listen(m_sSocket, nMaxSession);
	if (nError < 0)
	{
#ifdef _WIN32
        errno = WSAGetLastError();
#endif
		m_nLastError = errno;
		Close();
		return FALSE;
	}

	return TRUE;
}

SOCKET CTCPSocket::Accept(LPSTR pszAddress)
{
	struct	sockaddr_in	sin;
	SOCKET	sSocket;
	struct	linger 	Linger; 
	socklen_t	addrlen = sizeof(struct sockaddr_in);
//	BOOL	bNoDelay = TRUE;

	if (pszAddress)
		*pszAddress = '\0';

	// 소켓을 생성한다.
	sSocket = accept(m_sSocket, (struct sockaddr *)&sin, &addrlen);	
	if (sSocket == INVALID_SOCKET)
	{
#ifdef _WIN32
                errno = WSAGetLastError();
#endif
		m_nLastError = errno;
		return INVALID_SOCKET;
	}

//	setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, &bNoDelay, sizeof(bNoDelay));

	// 소켓을 완전히 닫게 하기 위하여
	Linger.l_onoff	= 1; 
	Linger.l_linger = 0; 
#ifndef _WIN32
 	setsockopt(sSocket, SOL_SOCKET, SO_LINGER, &Linger, sizeof(Linger));
#else
        setsockopt(sSocket, SOL_SOCKET, SO_LINGER, (const char *)&Linger, sizeof(Linger));
#endif

	if (pszAddress)
		strcpy(pszAddress, inet_ntoa((struct in_addr)sin.sin_addr));

	return sSocket;
}

BOOL CTCPSocket::Connect(const char* pszAddress, int nPort, BOOL bAsync)
{
	struct	sockaddr_in		addr;
	struct	hostent			*host;
	int		addrlen = sizeof(struct sockaddr_in);
#ifndef _WIN32
	int		nFlags;
#endif

	if (pszAddress)	strcpy(m_szAddress, pszAddress);
	if (nPort != -1) m_nPort = (WORD)nPort;
	
	memset(&addr, 0, addrlen);
	addr.sin_family			= AF_INET;
	addr.sin_port			= htons(m_nPort);
	addr.sin_addr.s_addr	= inet_addr(m_szAddress);

	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		host = gethostbyname(m_szAddress);
		if (host == NULL)
			return FALSE;

		addr.sin_addr.s_addr = (unsigned long)*((unsigned long *)host->h_addr);
	}

    //  [9/29/2010 DHKim]
#if !defined(__DISABLE_SSL__)
    if(m_nSecurityState == 1)
        bAsync = FALSE;
#endif

	if (bAsync)
	{
#ifdef _WIN32
        u_long on = 1;
#endif
		m_bAsyncMode = TRUE;
#ifdef _WIN32
        ioctlsocket(m_sSocket,FIONBIO,&on);
#else
		nFlags = fcntl(m_sSocket, F_GETFL, 0);
		fcntl(m_sSocket, F_SETFL, nFlags | O_NONBLOCK);
#endif
	}

	if ( connect(m_sSocket, (struct sockaddr *)&addr, addrlen) < 0)
	{
#ifdef _WIN32
        errno = WSAGetLastError();
#endif
//		printf("Connect errno = %d\r\n", errno);
		m_nLastError = errno;
#ifdef _WIN32
        if ((errno != WSAEINPROGRESS) && (errno != WSAEWOULDBLOCK))
#else
		if ((errno != EINPROGRESS) && (errno != EWOULDBLOCK))
#endif
			return FALSE;
	}

	return TRUE;
}

int CTCPSocket::Send(LPSTR pszBuffer, int nLength)
{
	if (m_sSocket == INVALID_SOCKET)
		return -1;

	return send(m_sSocket, pszBuffer, nLength, MSG_DONTWAIT);
}

int CTCPSocket::Receive(LPSTR pszBuffer, int nMaximum)
{
	if (m_sSocket == INVALID_SOCKET)
		return -1;

	return recv(m_sSocket, pszBuffer, nMaximum, 0);
}

