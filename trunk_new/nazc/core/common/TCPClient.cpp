//////////////////////////////////////////////////////////////////////
//
//    TCPClient.cpp: implementation of the CTCPClient class.
//
//    This file is a part of the AIMIR-FCP.
//    (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//    This source code can only be used under the terms and conditions 
//    outlined in the accompanying license agreement.
//
//    casir@com.ne.kr
//    http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

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
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#include "CommonUtil.h"
#include "TCPClient.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////////
//  [10/6/2010 DHKim]
//  extern data
//////////////////////////////////////////////////////////////////////////
#if !defined(__DISABLE_SSL__)
extern BYTE    m_nSecurityState;
BOOL		m_bSSLConnect = FALSE;
SSL_METHOD	*m_meth = NULL;
#endif

//////////////////////////////////////////////////////////////////////
// CTCPClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPClient::CTCPClient()
{
    m_bConnected         = FALSE;
    m_bReadySend        = FALSE;
    m_bReadyRecv        = FALSE;
    m_bExitSendPending    = FALSE;
    m_bExitRecvPending    = FALSE;
    m_nTID                = 0;
	m_ctx						= NULL;
	m_ssl						= NULL;

    sem_init(&m_sendSem, 0, 0);
}

CTCPClient::~CTCPClient()
{
    Disconnect();

    sem_destroy(&m_sendSem);
}

//////////////////////////////////////////////////////////////////////
// CTCPClient Attribute
//////////////////////////////////////////////////////////////////////

BOOL CTCPClient::IsConnected() const
{
    return m_bConnected;
}

//////////////////////////////////////////////////////////////////////
// CTCPClient Operations
//////////////////////////////////////////////////////////////////////

//  [10/7/2010 DHKim]
#if !defined(__DISABLE_SSL__)
BOOL CTCPClient::SSLClient_Initialize()
{
	SSL_library_init();
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();

	m_meth = SSLv3_client_method(); // meth = SSLv3_method();
	if(m_meth == NULL)
	{
		XDEBUG("SSLWrapper: Cannot find method (method=%p)\xd\xa", m_meth);
		return FALSE;
	}

	return TRUE;
}
#endif

BOOL CTCPClient::Connect(const char* pszAddress, int nPort, BOOL bAsync)
{
#if !defined(__DISABLE_SSL__)
	time_t	tmStart, now;
    int nError;
#endif
	int nResult;
    if (IsConnected())
        Disconnect();

    m_bConnected         = FALSE;
    m_bReadySend        = FALSE;
    m_bReadyRecv        = FALSE;
    m_bExitSendPending    = FALSE;
    m_bExitRecvPending    = FALSE;

    RemoveAllSendStream();
    if (!CTCPSocket::Create())
        return FALSE;

    CTCPSocket::SetAddress(pszAddress);
    CTCPSocket::SetPort(nPort);

	nResult = CTCPSocket::Connect(NULL, -1);
#if !defined(__DISABLE_SSL__)
	if (m_nSecurityState == 0)
    {
#endif
		if(nResult == -1)
		{
			//XDEBUG("ConnectErrorHandler Enter\r\n");    // DHKim
			ConnectErrorHandler();
			CTCPSocket::Close();
			return FALSE;
		}
#if !defined(__DISABLE_SSL__)
    }
	else
	{
		//  [9/14/2010 DHKim]
		if(m_nSecurityState == 1)
		{
			//XDEBUG("Enter SSL Mode: Socket Connect, port Function\r\n");
			nPort = 3939;

			//XDEBUG("Enter SSL Mode: Socket Connect, SSL Main Function\r\n");

			if( (m_ctx = SSL_CTX_new (m_meth)) == NULL)
			{
				if(m_meth == NULL)
				{
					//XDEBUG("method not initialize!!\r\n");
					SSLClient_Initialize();
				}
				XDEBUG("SSL CTX Context ERROR!!\r\n");
				//return FALSE;
			}
			if ( (m_ctx = SSL_CTX_new(m_meth)) == NULL)
			{
				XDEBUG("Try again: SSL CTX Create error!!\r\n");
				return FALSE;
			}

			m_ssl = SSL_new(m_ctx);
			if (m_ssl == NULL)
			{
				XDEBUG("SSL new Error!!\r\n");
				//ConnectErrorHandler();
				//CTCPSocket::Close();
				return FALSE;
			}

			if(SSL_set_fd(m_ssl, (SOCKET)CTCPSocket::GetHandle()) <= 0)
			{
				//ConnectErrorHandler();
				//CTCPSocket::Close();
				SSL_free(m_ssl);
				m_ssl = NULL;
				return FALSE;
			}

			time(&tmStart);
			for(;;)
			{
				time(&now);
				if ((now-tmStart) > 5)
				{
					// Connection timeout
					XDEBUG(" -------> SSL_connect() timeout!!\xd\xa");
					//ConnectErrorHandler();
					//CTCPSocket::Close();
					SSL_free(m_ssl);
					m_ssl = NULL;
					return FALSE;
				}

				//XDEBUG("SSL_connect()\xd\xa");
				nResult = SSL_connect(m_ssl);
				XDEBUG("SSL_connect() Result = %0d\xd\xa", nResult);
				if (nResult > 0)
				{
					m_bSSLConnect = TRUE;
					XDEBUG("SSL: SSL_connect continue ..\xd\xa");
					break;
				}

				// SSL_connect fail
				nError = SSL_get_error(m_ssl, nResult);
				XDEBUG("SSL: SSL_connect error!! (nResult=%d, nError=%d)\xd\xa", nResult, nError);

				if ((nError != SSL_ERROR_WANT_READ) && (nError != SSL_ERROR_WANT_WRITE))
				{
					//ConnectErrorHandler();
					//CTCPSocket::Close();
					SSL_free(m_ssl);
					m_ssl = NULL;
					return FALSE;
				}
				else
				{
					m_bSSLConnect = TRUE;
					XDEBUG("SSL: SSL_connect continue ..\xd\xa");
					USLEEP(100000);
				}
			}
		}
	}
#endif

	//////////////////////////////////////////////////////////////////////////

    /*
#ifdef _WIN32
	m_hWatchThread	= CreateThread(NULL, 
									4096, 
									(LPTHREAD_START_ROUTINE)WatchThread, 
									(LPVOID)this, 
									CREATE_SUSPENDED,
									&m_dwWatchThreadID);
	SetThreadPriority(m_hWatchThread, THREAD_PRIORITY_NORMAL);

	m_hSendThread	= CreateThread(NULL, 
									4096, 
									(LPTHREAD_START_ROUTINE)SendThread, 
									(LPVOID)this, 
									CREATE_SUSPENDED,
									&m_dwSendThreadID);
	SetThreadPriority(m_hSendThread, THREAD_PRIORITY_NORMAL);

	ResumeThread(m_hWatchThread);
	ResumeThread(m_hSendThread);
#else
*/
	if (pthread_create(&m_watchThreadID, NULL, WatchThread, (void *)this) != 0)
		return FALSE;

    if (pthread_create(&m_sendThreadID, NULL, SendThread, (void *)this) != 0)
        return FALSE;
    /*
#endif
*/

    m_bConnected = TRUE;
    while(!m_bReadySend    || !m_bReadyRecv)
        USLEEP(100000);

    ConnectHandler();
    return TRUE;
}

void CTCPClient::Disconnect()
{
    int        i;
    void *   nStatus;

    if (!IsConnected())
        return;

    // 전송할 항목이 남아 있으면 최대 1초 동안 대기한다.
    for(i=0; i<30; i++)
    {
        if (!IsPendingSendQ())
            break;

        USLEEP(30000);
    }

    // Terminate Send Thread
    m_bExitSendPending = TRUE;
    sem_post(&m_sendSem);
    pthread_join(m_sendThreadID, &nStatus);

    // Terminate Watch Thread
    m_bExitRecvPending = TRUE;
//    CTCPSocket::Shutdown();
    pthread_join(m_watchThreadID, &nStatus);

	//  [10/6/2010 DHKim]
#if !defined(__DISABLE_SSL__)
	if(m_nSecurityState == 1)
	{
		if(m_ssl != NULL)
		{
			SSL_free(m_ssl);
			m_ssl = NULL;
		}
	}
#endif
	//////////////////////////////////////////////////////////////////////////

    CTCPClient::CloseHandler();
    RemoveAllSendStream();

    m_bExitSendPending     = FALSE;
    m_bExitRecvPending     = FALSE;
    m_bConnected        = FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTCPClient Member Functions
//////////////////////////////////////////////////////////////////////

// Send Stream Functions ---------------------------------------------

TID CTCPClient::AddSendStream(LPSTR pszBuffer, int nLength)
{
    TCPDATASTREAM    *pStream;

    if (pszBuffer == NULL)
        return -1;

    nLength                = (nLength == -1) ? strlen(pszBuffer) : nLength;
    pStream                = (TCPDATASTREAM *)MALLOC(sizeof(TCPDATASTREAM));
    pStream->nLength    = nLength;
    pStream->pszBuffer    = (char *)MALLOC(nLength);
    memcpy(pStream->pszBuffer, pszBuffer, nLength);

    m_SendLocker.Lock();
    pStream->nTID        = m_nTID++;
    pStream->nPosition  = (int)m_SendList.AddTail(pStream);
    m_SendLocker.Unlock();

    sem_post(&m_sendSem);
    return pStream->nTID;
}

void CTCPClient::FreeStream(TCPDATASTREAM *pStream)
{
    if (!pStream)
        return;

    if (pStream->pszBuffer) FREE(pStream->pszBuffer);
    FREE(pStream);
}

BOOL CTCPClient::IsPendingSendQ()
{
    BOOL    bPending;

    m_SendLocker.Lock();
    bPending = m_SendList.GetCount() > 0 ? TRUE : FALSE;
    m_SendLocker.Unlock();

    return bPending;
}

TCPDATASTREAM *CTCPClient::GetStream()
{
    TCPDATASTREAM    *pStream;

    m_SendLocker.Lock();
    pStream = m_SendList.GetHead();
    if (pStream) m_SendList.RemoveAt((POSITION)pStream->nPosition);
    m_SendLocker.Unlock();

    return pStream;
}

void CTCPClient::RemoveAllSendStream()
{
    TCPDATASTREAM    *pStream, *pCurrent;
    POSITION        pos;

    m_SendLocker.Lock();

    pStream = m_SendList.GetFirst(pos);
    while(pStream)
    {
        pCurrent = pStream;
        pStream  = m_SendList.GetNext(pos);

        FreeStream(pCurrent);
    }
    m_SendList.RemoveAll();

    m_SendLocker.Unlock();
}

int CTCPClient::SendStream(char *pszBuffer, int nLength)
{
#if !defined(__DISABLE_SSL__)
    int         nError;
#endif
    int        nCount=0, nSend=0;

    for(;(nSend<nLength) && !m_bExitSendPending;)
    {
        //XDEBUG("SendStream!!!\r\n");            //DHKim
#if !defined(__DISABLE_SSL__)
		if(m_nSecurityState == 1 && m_bSSLConnect)
		{
			nCount = SSL_write(m_ssl, pszBuffer+nSend, nLength-nSend);
			if (nCount <= 0)
			{
				nError = SSL_get_error(m_ssl, nCount);
				if ((nError != SSL_ERROR_WANT_READ) && (nError != SSL_ERROR_WANT_WRITE))
				{
					XDEBUG("SSL SEND ERROR (nCount=%d, Error=%d)\xd\xa", nCount, nError);
					return -1;
				}
				USLEEP(100000);
				continue;
			}
		}
		else if(m_nSecurityState == 0)
		{
#endif
			nCount = CTCPSocket::Send(pszBuffer+nSend, nLength-nSend);
			if (nCount <= 0)
			{
				//            printf("Send=%d errno=%d\r\n", nCount, errno);
#ifndef _WIN32
				if ((errno == EAGAIN) || (errno == EINPROGRESS) || (errno == EWOULDBLOCK))
#else
				if ((errno == EAGAIN) || (errno == WSAEINPROGRESS) || (errno == WSAEWOULDBLOCK))
#endif
				{
					USLEEP(100000);    // 10 ms
					continue;
				}
				return -1;
			}
#if !defined(__DISABLE_SSL__)
		}
#endif
//        printf("Send=%d errno=%d\r\n", nCount, errno);
        nSend += nCount;
    }

    return nSend;
}

// Socket Handler Functions ------------------------------------------

BOOL CTCPClient::ConnectHandler()
{
	m_bConnected = TRUE;

    OnConnect();
    return TRUE;
}

void CTCPClient::CloseHandler()
{
    if (IsConnected())
        OnDisconnect();

//    m_bExitPending = TRUE;
//    sem_post(&m_sendSem);

    CTCPSocket::Close();
    m_bConnected = FALSE;
}

void CTCPClient::ConnectErrorHandler()
{
    OnConnectError();
}

BOOL CTCPClient::SendHandler()
{
    TCPDATASTREAM    *pStream;
    int                nSend = 0;

    pStream = GetStream();
    while(pStream && !m_bExitSendPending)
    {
        nSend = SendStream(pStream->pszBuffer, pStream->nLength);
        if (nSend > 0)
            OnSend(pStream->pszBuffer, nSend);
        FreeStream(pStream);

        if (nSend == -1)
        {
            ErrorHandler(TCPIO_TYPE_SEND);
//            return FALSE;
            return TRUE;
        }
        
        pStream = GetStream();
    }

    return TRUE;
}

BOOL CTCPClient::ReceiveHandler()
{
#if !defined(__DISABLE_SSL__)
    int         nError;
#endif
    int        nLength=0;

#if !defined(__DISABLE_SSL__)
	if(m_nSecurityState == 1 && m_bSSLConnect)
	{
		nLength = SSL_read(m_ssl, m_szBuffer, DEFAULT_SESSION_BUFFER);
		XDEBUG("SSL RECV DATA: %d\r\n", nLength);
		if (nLength <= 0)
		{
			nError = SSL_get_error(m_ssl, nLength);
			/*
			if (nError == SSL_ERROR_SSL)
			{
			SSL_free(c_ssl);
			c_ssl = SSL_new();
			SSL_set_fd(c_ssl, CTCPSocket::GetHandle());
			SSL_connect(c_ssl);
			usleep(100000);
			return TRUE;
			}
			*/
			if ((nError != SSL_ERROR_WANT_READ) && (nError != SSL_ERROR_WANT_WRITE))
			{
				XDEBUG("SSL RECV ERROR (nLength=%d, nError=%d)\xd\xa", nLength, nError);
				//CloseHandler();
				return TRUE;
			}
			USLEEP(100000);
			return TRUE;
		}
	}
	else if(m_nSecurityState == 0)
	{
#endif
		nLength = CTCPSocket::Receive(m_szBuffer, DEFAULT_SESSION_BUFFER);
		//XDEBUG("RECV Data: %d\r\n", nLength);
		if (nLength <= 0)
		{
			//        printf("RECV errno = %d, len=%d\r\n", errno, nLength);
#ifndef _WIN32
            if ((errno == EAGAIN) || (errno == EINPROGRESS) || (errno == EWOULDBLOCK))
#else
            if ((errno == EAGAIN) || (errno == WSAEINPROGRESS) || (errno == WSAEWOULDBLOCK))
#endif
			{
				if (nLength == -1)
				{
					USLEEP(30000);    // 10 ms
					return TRUE;
				}
			}

//        printf("RECV error (Length=%d, errno=%d)\r\n", nLength, errno);
//        USLEEP(1000000);

//        CloseHandler();
//        return FALSE;
			return TRUE;
		}
#if !defined(__DISABLE_SSL__)
    }
#endif

    //printf("RECV len=%d\r\n", nLength);
    return OnReceive(m_szBuffer, nLength);
}

void CTCPClient::ErrorHandler(int nType)
{
    OnError(nType);

//    m_bExitPending = TRUE;
//    CloseHandler();
}

//////////////////////////////////////////////////////////////////////
// CTCPClient Overrides
//////////////////////////////////////////////////////////////////////

// Socket Message Handlers -------------------------------------------

void CTCPClient::OnConnect()
{
//    XDEBUG("CTCPClient::OnConntect\r\n");
}

void CTCPClient::OnDisconnect()
{
//    XDEBUG("CTCPClient::OnDisconntect\r\n");
}

void CTCPClient::OnConnectError()
{
}

void CTCPClient::OnSend(LPSTR pszBuffer, int nLength)
{
}

BOOL CTCPClient::OnReceive(LPSTR pszBuffer, int nLength)
{
    return TRUE;
}

void CTCPClient::OnError(int nType)
{
}

// Thread Message Handlers -------------------------------------------

void CTCPClient::RunWatchThread()
{
    BOOL    bForever = TRUE;

    m_bReadyRecv = TRUE;
    for(;bForever && !m_bExitRecvPending;)
    {
        bForever = ReceiveHandler();
    }
}

void CTCPClient::RunSendThread()
{
    BOOL    bForever = TRUE;

    m_bReadySend = TRUE;
    for(;bForever && !m_bExitRecvPending;)
    {
        sem_wait(&m_sendSem);
        if (m_bExitSendPending)
            break;

        bForever = SendHandler();
    }
}

//////////////////////////////////////////////////////////////////////
// CTCPClient Overrides
//////////////////////////////////////////////////////////////////////

void *CTCPClient::WatchThread(void *pParam)
{
    CTCPClient    *pThis;

    pThis = (CTCPClient *)pParam;
    pThis->RunWatchThread();

    return NULL;
}

void *CTCPClient::SendThread(void *pParam)
{
    CTCPClient    *pThis;

    pThis = (CTCPClient *)pParam;
    pThis->RunSendThread();
    pThis->RemoveAllSendStream();

    return NULL;
}
