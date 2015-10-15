//////////////////////////////////////////////////////////////////////
//
//	TCPMultiplexServer.cpp: implementation of the CTCPMultiplexServer class.
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
#ifndef _WIN32
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

#include "TCPMultiplexServer.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//  [9/9/2010 DHKim]
#define CERTF "cert.pem"
#define KEYF "server.pem"

#ifndef _WIN32
#if !defined(__DISABLE_SSL__)
void  ssl_info_callback(const SSL *s, int where, int ret);
#endif
#endif

BIO *m_errBIO;

#if !defined(__DISABLE_SSL__)
extern BYTE m_nSecurityState;
BOOL    m_bSSLAccept = FALSE;
BOOL    m_blocalcommand = FALSE;

SSL_METHOD *     s_meth	= NULL;
#endif
/////////////////////////////////////////////////////////////////////
// CTCPMultiplexServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPMultiplexServer::CTCPMultiplexServer()
{
	m_bStarted		= FALSE;
	m_bExitPending	= FALSE;
	m_nMaxSession	= 0;
	m_nTimeout		= -1;
#if !defined(__DISABLE_SSL__)
	s_ctx				= NULL;
	s_ssl				= NULL;
#endif
	
	sem_init(&m_sendSem, 0, 0);
}

CTCPMultiplexServer::~CTCPMultiplexServer()
{
	Shutdown();

	sem_destroy(&m_sendSem);
}

//////////////////////////////////////////////////////////////////////
// CTCPMultiplexServer Attribute
//////////////////////////////////////////////////////////////////////

//  [10/7/2010 DHKim]
#if !defined(__DISABLE_SSL__)
BOOL CTCPMultiplexServer::SSLServer_Initialize()
{
	SSL_library_init();
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();

	s_meth = SSLv3_server_method(); // meth = SSLv3_method();
	if(s_meth == NULL)
	{
		XDEBUG("SSLWrapper: Cannot find method (method=%p)\xd\xa", s_meth);
		return FALSE;
	}

	return TRUE;
}
#endif
BOOL CTCPMultiplexServer::IsStarted() const
{
	return m_bStarted;
}

BOOL CTCPMultiplexServer::IsSession()
{
	return (m_SessionList.GetCount() > 0) ? TRUE : FALSE;
}

int CTCPMultiplexServer::GetMaxSession()
{
	return m_nMaxSession;
}

int CTCPMultiplexServer::GetSessionCount()
{
	return m_SessionList.GetCount();
}

int CTCPMultiplexServer::GetTimeout() const
{
	return m_nTimeout;
}

void CTCPMultiplexServer::SetTimeout(int nTimeout)
{
	m_nTimeout = nTimeout;
}

BOOL CTCPMultiplexServer::GetSession(WORKSESSION *pFind)
{
	WORKSESSION		*pSession;
	POSITION		pos=INITIAL_POSITION;
	BOOL			bFind = FALSE;

	m_SessionLocker.Lock();
	pSession = m_SessionList.GetFirst(pos);
	while(pSession)
	{
		if (pSession == pFind)
		{
			bFind = TRUE;
			break;
		}
		pSession = m_SessionList.GetNext(pos);
	}
	m_SessionLocker.Unlock();
	return bFind;
}

//////////////////////////////////////////////////////////////////////
// CTCPMultiplexServer Operations
//////////////////////////////////////////////////////////////////////

BOOL CTCPMultiplexServer::Startup(int nPort, int nMaxSession, int nTimeout)
{
	if (IsStarted())
		return FALSE;

	m_bStarted		= FALSE;
	m_bExitPending	= FALSE;
	m_nMaxSession	= nMaxSession;
	m_nTimeout		= nTimeout;

#if !defined(__DISABLE_SSL__)
    // [9/20/2010 DHKim]
    XDEBUG("MultiplexServer start: %d\r\n", m_nSecurityState);
#endif

	if (!CreateDaemonSocket(nPort, nMaxSession))
		return FALSE;

	if (pthread_create(&m_watchThreadID, NULL, WatchThread, (void *)this) != 0)
		return FALSE;

	if (pthread_create(&m_sendThreadID, NULL, SendThread, (void *)this) != 0)
		return FALSE;

	m_bStarted = TRUE;
	return TRUE;
}

void CTCPMultiplexServer::Shutdown()
{
	void	*nStatus;

	if (!IsStarted())
		return;

	m_bExitPending	= TRUE;

	// 전송 쓰레드 종료
	sem_post(&m_sendSem);
 	pthread_join(m_sendThreadID, &nStatus);

	// 수신 쓰레드 종료
	CTCPSocket::Shutdown();
 	pthread_join(m_watchThreadID, &nStatus);
	CloseDaemonSocket();

	m_bStarted	= FALSE;
}

void CTCPMultiplexServer::Broadcast(const char *pszBuffer, int nLength)
{
	WORKSESSION	*pSession;
	POSITION	pos;

	m_SessionLocker.Lock();
	pSession = m_SessionList.GetFirst(pos);
	while(pSession)
	{
		if (pSession->sSocket != INVALID_SOCKET)
			AddSendStream(pSession, pszBuffer, nLength);

		pSession = m_SessionList.GetNext(pos);
	}
	m_SessionLocker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// CTCPMultiplexServer Member Functions
//////////////////////////////////////////////////////////////////////

BOOL CTCPMultiplexServer::CreateDaemonSocket(int nPort, int nMaxSession)
{
	if (!CTCPSocket::Create())
		return FALSE;

    //////////////////////////////////////////////////////////////////////////
    //  [9/9/2010 DHKim]
    //  SSL 추가
    //////////////////////////////////////////////////////////////////////////
#if !defined(__DISABLE_SSL__)
    if(m_nSecurityState == 1)
    {
        //XDEBUG("Enter SSL Mode: CreateDaemonSocket Function\r\n");

        s_ctx = SSL_CTX_new (s_meth);
        if (!s_ctx) {
            ERR_print_errors_fp(stderr);
            return FALSE;
        }
        // SSL 핸드쉐이크 메시지교환 과정을 알려주는 콜벡함수
        //SSL_CTX_set_info_callback(s_ctx, ssl_info_callback);
        
        // 자신의 인증서를 파일에서 로딩한다.
        if (SSL_CTX_use_certificate_file(s_ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            return FALSE;
        }
        
        fprintf(stderr, "======== PEM pass phrease confirm here\n");
        
        // 자신의 개인키를 파일에서 로딩한다.
        if (SSL_CTX_use_PrivateKey_file(s_ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
            fprintf(stderr,"======== PEM pass phrase does not match the password\n");
            ERR_print_errors_fp(stderr);
            return FALSE;
        }
        
        // 읽은 인증서와 개인키가 맞는지 확인 한다.
        if (!SSL_CTX_check_private_key(s_ctx)) {
            fprintf(stderr,"Private key does not match the certificate public key\n");
            return FALSE;
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////

    //  [9/14/2010 DHKim]
#if !defined(__DISABLE_SSL__)
    if(m_nSecurityState == 1)
    {
       // XDEBUG("Enter SSL Mode: Socket Bind Function\r\n");

        if (!CTCPSocket::Bind(3939))
        {
            CTCPSocket::Close();
            return FALSE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    else
    {
#endif
	    if (!CTCPSocket::Bind(nPort))
	    {
		    CTCPSocket::Close();
		    return FALSE;
	    }
#if !defined(__DISABLE_SSL__)
    }
#endif

	if (!CTCPSocket::Listen(nMaxSession))
	{
		CTCPSocket::Close();
		return FALSE;
	}

	return TRUE;
}

void CTCPMultiplexServer::CloseDaemonSocket()
{
	CTCPSocket::Close();
	DeleteAllSession();
}

WORKSESSION *CTCPMultiplexServer::NewSession(SOCKET sSocket, LPSTR pszAddress)
{
	WORKSESSION	*pSession;

	pSession = (WORKSESSION *)MALLOC(sizeof(WORKSESSION));
	if (!pSession)
		return NULL;

	memset(pSession, 0, sizeof(WORKSESSION));
	time(&pSession->lLastInput);
	pSession->pThis		= (void *)this;
	pSession->sSocket	= sSocket;
	strcpy(pSession->szAddress, pszAddress);

	m_SessionLocker.Lock();
	pSession->nPosition = (int)m_SessionList.AddTail(pSession);
	m_SessionLocker.Unlock();

	return pSession;
}

void CTCPMultiplexServer::DeleteSession(WORKSESSION *pSession)
{
	if (!pSession)
		return;

	m_SessionLocker.Lock();
	m_SessionList.RemoveAt((POSITION)pSession->nPosition);
	m_SessionLocker.Unlock();

	if (pSession->sSocket != INVALID_SOCKET)
	{
		OnCloseSession(pSession);
#ifdef _WIN32
        closesocket(pSession->sSocket);
#else
		close(pSession->sSocket);
#endif
		pSession->sSocket = INVALID_SOCKET;
	}

	FREE(pSession);
}

void CTCPMultiplexServer::DeleteAllSession()
{
	WORKSESSION	*pSession, *pDeleteIt;
	POSITION	pos;

	m_SessionLocker.Lock();
	pSession = m_SessionList.GetFirst(pos);
	while(pSession)
	{
		pDeleteIt = pSession;		
		pSession = m_SessionList.GetNext(pos);
		
		if (pDeleteIt->sSocket != INVALID_SOCKET)
		{
			OnCloseSession(pDeleteIt);
#ifdef _WIN32
            closesocket(pDeleteIt->sSocket);
#else
			close(pDeleteIt->sSocket);
#endif
			pDeleteIt->sSocket = INVALID_SOCKET;
		}

		FREE(pDeleteIt);
	}
	m_SessionList.RemoveAll();
	m_SessionLocker.Unlock();
}

TID CTCPMultiplexServer::AddSendStream(WORKSESSION *pSession, const char *pszBuffer, int nLength)
{
	TCPDATASTREAM	*pStream;

	if (!pszBuffer)
		return 0;

	nLength = (nLength == -1) ? strlen(pszBuffer) : nLength;
	if (nLength == 0)
		return 0;

	pStream = (TCPDATASTREAM *)MALLOC(sizeof(TCPDATASTREAM));
	if (pStream == NULL)
		return -1;

	memset(pStream, 0, sizeof(TCPDATASTREAM));
	pStream->nLength	= nLength;
	pStream->pSession	= (void *)pSession;
	pStream->pszBuffer	= (char *)MALLOC(nLength);
	memcpy(pStream->pszBuffer, pszBuffer, nLength);

	m_StreamLocker.Lock();
	pStream->nPosition = (int)m_StreamList.AddTail(pStream);
	m_StreamLocker.Unlock();

	sem_post(&m_sendSem);
	return 0;
}

void CTCPMultiplexServer::RemoveAllStream(WORKSESSION *pSession)
{
	TCPDATASTREAM	*pStream, *pDelete;
	POSITION		pos;

	m_StreamLocker.Lock();
	pStream = m_StreamList.GetFirst(pos);
	while(pStream)
	{
		pDelete = pStream;
		pStream = m_StreamList.GetNext(pos);

		if ((pSession == NULL) || (pDelete->pSession == (void *)pSession))
		{
			m_StreamList.RemoveAt((POSITION)pDelete->nPosition);
			if (pDelete->pszBuffer)
				FREE(pDelete->pszBuffer);
			FREE(pDelete);
		}
	}
	if (pSession == NULL)
		m_StreamList.RemoveAll();
	m_StreamLocker.Unlock();
}

int CTCPMultiplexServer::SendStreamToSession(SOCKET sSocket, char *pszBuffer, int nLength)
{
	int		nCount=0, nSend=0;

	for(;nSend<nLength;)
	{
		try
		{
#if !defined(__DISABLE_SSL__)
			if(m_nSecurityState == 1 && m_bSSLAccept && !m_blocalcommand)
			{
				//XDEBUG("Enter SSL Mode: Socket Send Function\r\n");
				nCount = SSL_write(s_ssl, pszBuffer, nLength);
				
				switch(SSL_get_error(s_ssl, nCount))
				{
				case SSL_ERROR_NONE:
					XDEBUG("SSL write Success!! len: %d\r\n", nCount);
					XDUMP(pszBuffer,nLength,TRUE);
					break;
				case SSL_ERROR_ZERO_RETURN:
					XDEBUG("SSL Shutdown!!\r\n");
					SSL_shutdown(s_ssl);
					break;
				}
			}
			else if(m_nSecurityState == 0 || m_blocalcommand)
#endif
				nCount = send(sSocket, pszBuffer+nSend, nLength-nSend, 0);
		}
		catch(...)
		{
			printf("send error~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~!!!!!\xd\xa");
			return -1;
		}
		if (nCount <= 0)
			return -1;

		nSend += nCount;
	}

	return nSend;
}

int CTCPMultiplexServer::SendToSession(WORKSESSION *pSession, const char *pszBuffer, int nLength)
{
	nLength = (nLength == -1) ? strlen(pszBuffer) : nLength;
	return AddSendStream(pSession, pszBuffer, nLength);
}

int CTCPMultiplexServer::SendToSession(WORKSESSION *pSession, char c)
{
	char	szBuffer[1];

	szBuffer[0] = c;
	return SendToSession(pSession, szBuffer, 1);
}

void CTCPMultiplexServer::SessionError(WORKSESSION *pSession, int nType)
{
	OnErrorSession(pSession, nType);
}

//////////////////////////////////////////////////////////////////////////
//  [9/9/2010 DHKim]
//  SSL 핸드쉐이크 메시지교환 과정을 알려주는 콜벡함수
//////////////////////////////////////////////////////////////////////////

#if !defined(__DISABLE_SSL__)
void ssl_info_callback(const SSL *s, int where, int ret)
{
    const char* writeString;
    int     w;
    w = where & ~SSL_ST_MASK;

    // 클라이언트가 연결 했을 때
    if(w & SSL_ST_CONNECT)
        writeString = "SSL_connect";
    // 서버가 연결을 받았을 때
    else if(w & SSL_ST_ACCEPT)
        writeString = "SSL_accept";
    else if (w & SSL_ST_INIT)
        writeString = "SSL_INIT";
    else if (w & SSL_ST_BEFORE)
        writeString = "SSL_before";
    // 알 수 없는 경우
    else
        writeString = "undefined";

    fprintf(stderr, "======== writeString = [%s]\n", writeString);

    // 일반적인 핸드쉐이크 프로토콜 메시지일 경우
    if (where & SSL_CB_LOOP)
    {
        // SSL_state_string_long(s) 함수로 부터 현재 진행되는 메시지가 무엇인지 표시
        BIO_printf(m_errBIO,"%s:%s\n",writeString,SSL_state_string_long(s));
        fprintf(stderr, "======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", writeString, SSL_state_string_long(s));
    }
    else if (where & SSL_CB_ALERT)
    {
        // alert 프로토콜일 경우
        writeString=(where & SSL_CB_READ)?"read":"write";
        BIO_printf(m_errBIO,"SSL3 alert %s:%s:%s\n",writeString,SSL_alert_type_string_long(ret),SSL_alert_desc_string_long(ret));
        fprintf(stderr, "======== writeString = [%s], SSL_alert_type_string_long(ret) = [%s], SSL_alert_desc_string_long(ret) = [%s]\n", writeString, SSL_alert_type_string_long(ret), SSL_alert_desc_string_long(ret));
    }
    else if (where & SSL_CB_EXIT)
    {
        // 종료 과정일 경우
        if (ret == 0) {
            BIO_printf(m_errBIO,"%s:failed in %s\n",writeString,SSL_state_string_long(s));
            fprintf(stderr,"======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", writeString, SSL_state_string_long(s));
        }
        else if (ret < 0)
        {
            BIO_printf(m_errBIO,"%s:error in %s\n",writeString,SSL_state_string_long(s));
            fprintf(stderr,"======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", writeString, SSL_state_string_long(s));
        }
    }
}
#endif


//////////////////////////////////////////////////////////////////////
// CTCPMultiplexServer Override Functions
//////////////////////////////////////////////////////////////////////

// CTCPMultiplexServer Daemon Override Functions ------------------------------

BOOL CTCPMultiplexServer::EnterDaemon()
{
	return OnEnterDaemon();
}

void CTCPMultiplexServer::LeaveDaemon()
{
	CloseDaemonSocket();
	DeleteAllSession();

	OnLeaveDaemon();
}

BOOL CTCPMultiplexServer::RunDaemon()
{
	WORKSESSION		*pSession = NULL;
	struct	timeval	timeout;
	fd_set	reads, temps;
	char	szBuffer[DEFAULT_SESSION_BUFFER+1];
#ifdef _WIN32
    SOCKET		fd_max, fd;
#else
	int		fd_max, fd;
#endif
	int		len;
	long	lCurTime;
	BOOL	bForever = TRUE;

	FD_ZERO(&reads);
	FD_SET(m_sSocket, &reads);
	fd_max	= m_sSocket;
	
	for(;bForever && !m_bExitPending;)
	{
		temps = reads;
		timeout.tv_sec	= 5;
		timeout.tv_usec	= 0;

try
{	
		if (select(fd_max+1, &temps, NULL, NULL, &timeout) == -1)
		{
			printf("*****************************\xd\xa");
			printf("******* Select() error ******\xd\xa");
			printf("*****************************\xd\xa");
		}

		time(&lCurTime);
		for(fd=0; fd<fd_max+1; fd++)
		{
			if (FD_ISSET(fd, &temps))
			{
				// Connect New Session 
				if (fd == m_sSocket)
				{
					pSession = AcceptSession();
					if (pSession != NULL)
					{
						FD_SET(pSession->sSocket, &reads);
						if (fd_max < pSession->sSocket)
							fd_max = pSession->sSocket;
						EnterSession(pSession);
					}
				}
				else
				{
#if !defined(__DISABLE_SSL__)
					if(m_nSecurityState == 1 && !m_blocalcommand && m_bSSLAccept)
					{
						//XDEBUG("Multiplexer Server SSL read!!\r\n");
						len = SSL_read(s_ssl, szBuffer, DEFAULT_SESSION_BUFFER);
						
						switch(SSL_get_error(s_ssl, len))
						{
						case SSL_ERROR_NONE:
							XDEBUG("SSL read Success!! len: %d\r\n", len);
							XDUMP(szBuffer, len, TRUE);
							break;
						case SSL_ERROR_ZERO_RETURN:
							XDEBUG("SSL Shutdown!!\r\n");
							SSL_shutdown(s_ssl);
							break;
						}
					}
					else if(m_nSecurityState == 0 || m_blocalcommand)
					{
#endif
						//XDEBUG("Multiplexer receive normal!!\r\n");
						len	= recv(fd, szBuffer, DEFAULT_SESSION_BUFFER, 0);
#if !defined(__DISABLE_SSL__)
					}
					else
					{
						//XDEBUG("receive data error!!\r\n");
						len = 0;
					}
#endif

					if (len <= 0)
					{
						// Close Session 
						pSession = FindSession(fd);
						if (pSession)
						{
							LeaveSession(pSession);
#if !defined(__DISABLE_SSL__)
							if(!m_blocalcommand && m_nSecurityState == 1)
							{
								SSL_set_accept_state(s_ssl);        // [10/05/2010 DHKim]
								m_bSSLAccept = FALSE;               // [10/05/2010 DHKim]
							}
#endif
						}
						FD_CLR(fd, &reads);
					}
					else
					{	
						// Receive Session
						pSession = FindSession(fd);
						if (pSession)
						{
							pSession->lLastInput = lCurTime;
							if (!AddSessionRecvStream(pSession, szBuffer, len))
							{
								pSession = FindSession(fd);
								if (pSession)
								{
									LeaveSession(pSession);
#if !defined(__DISABLE_SSL__)
									if(!m_blocalcommand && m_nSecurityState == 1)
									{
										SSL_set_accept_state(s_ssl);        // [10/05/2010 DHKim]
										m_bSSLAccept = FALSE;               // [10/05/2010 DHKim]
									}
#endif
								}
								FD_CLR(fd, &reads);
							}
						}
					}
				}
			}
			else if (m_nTimeout != -1)
			{
				pSession = FindSession(fd);
				if (pSession)
				{
					if ((lCurTime-pSession->lLastInput) >= m_nTimeout)
					{
						pSession->lLastInput = lCurTime;
						if (!OnTimeoutSession(pSession))
						{
							// Close Session 
							LeaveSession(pSession);
#if !defined(__DISABLE_SSL__)
							if(!m_blocalcommand && m_nSecurityState == 1)
							{
								SSL_set_accept_state(s_ssl);        // [10/05/2010 DHKim]
								m_bSSLAccept = FALSE;               // [10/05/2010 DHKim]
							}
#endif
							FD_CLR(fd, &reads);
						}
					}
				}
			}
		}
}
catch(...)
{
	printf("error~~~~~~~~~~~~~~~~~~~~~~~!!!!\xd\xa");
}
			
	}

	return FALSE;
}

// CTCPMultiplexServer Daemon Session Functions -------------------------------

WORKSESSION *CTCPMultiplexServer::AcceptSession()
{
	WORKSESSION			*pSession;
	SOCKET				sSocket;
	char				szAddress[32];

	sSocket = CTCPSocket::Accept(szAddress);
	if (sSocket == INVALID_SOCKET)
		return NULL;

#if !defined(__DISABLE_SSL__)
    //  [9/10/2010 DHKim]
    if(strcmp(szAddress, "127.0.0.1") == 0)
        m_blocalcommand = TRUE;
    else
        m_blocalcommand = FALSE;

    if(m_nSecurityState == 1 && !m_bSSLAccept && !m_blocalcommand)
    {
        //XDEBUG("Enter SSL Mode: AcceptSession Function\r\n");
        //XDEBUG("Accept Address: %s\r\n", szAddress);

        if( (s_ssl = SSL_new(s_ctx)) == NULL)
        {
            XDEBUG("SSL new Error!!\r\n");
            return NULL;
        }

        if( SSL_set_fd(s_ssl, sSocket) <= 0 )
        {
            XDEBUG("SSL FD Error!!\r\n");
            return NULL;
        }

        if( SSL_accept(s_ssl) <= 0 )
        {
            XDEBUG("SSL accept Error!!\r\n");
            SSL_set_accept_state(s_ssl);
            return NULL;
        }
        else
        {
            XDEBUG("SSL accept success!!\r\n");
            m_bSSLAccept = TRUE;
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    
	if (GetSessionCount() >= m_nMaxSession)
	{
#ifdef _WIN32
        closesocket(sSocket);
#else
		close(sSocket);
#endif
		return NULL;
	}

	pSession = NewSession(sSocket, szAddress);
	if (!pSession)
	{
#ifdef _WIN32
        closesocket(sSocket);
#else
		close(sSocket);
#endif
		return NULL;
	}

	OnAcceptSession(pSession);
	return pSession;
}

BOOL CTCPMultiplexServer::EnterSession(WORKSESSION *pSession)
{
    //CheckPoint(&pSession->nDebugPoint);
	return OnEnterSession(pSession);
}

void CTCPMultiplexServer::LeaveSession(WORKSESSION *pSession)
{
    //unsigned int nPoint = pSession->nDebugPoint;
	OnLeaveSession(pSession);
	DeleteSession(pSession);
    //Difference(nPoint);
}

WORKSESSION *CTCPMultiplexServer::FindSession(SOCKET sSocket)
{
	WORKSESSION		*pSession;
	POSITION		pos=INITIAL_POSITION;

	m_SessionLocker.Lock();
	pSession = m_SessionList.GetFirst(pos);
	while(pSession)
	{
		if (pSession->sSocket == sSocket)
			break;
		pSession = m_SessionList.GetNext(pos);
	}
	m_SessionLocker.Unlock();

	return pSession;
}

BOOL CTCPMultiplexServer::AddSessionRecvStream(WORKSESSION *pSession, char *pszBuffer, int nLength)
{
	return OnReceiveSession(pSession, pszBuffer, nLength);
}

TCPDATASTREAM *CTCPMultiplexServer::GetStream()
{
	TCPDATASTREAM	*pStream;

	m_StreamLocker.Lock();
	pStream = m_StreamList.GetHead();
	if (pStream)
		m_StreamList.RemoveAt((POSITION)pStream->nPosition);
	m_StreamLocker.Unlock();

	return pStream;
}

BOOL CTCPMultiplexServer::SessionSender()
{
	WORKSESSION		*pSession;
	TCPDATASTREAM	*pStream;
	BOOL			bForever = TRUE;

    for(;bForever && !m_bExitPending;)
	{
		sem_wait(&m_sendSem);
		
		if (m_bExitPending)
			return FALSE;

		while(bForever && !m_bExitPending)
		{
			pStream = GetStream();
			if (pStream == NULL)
				break;

			pSession = (WORKSESSION *)pStream->pSession;
			if (pSession != NULL)
			{
				try
				{
					if (SendStreamToSession(pSession->sSocket, pStream->pszBuffer, pStream->nLength) == -1)
						 SessionError(pSession, TCPIO_TYPE_SEND);
					else OnSendSession(pSession, pStream->pszBuffer, pStream->nLength);
				}
			  	catch(...)
				{
					printf("CLI Excption .................\xd\xa");
//					SessionError(pSession, TCPIO_TYPE_SEND);
				}
			}

			if (pStream->pszBuffer)
				FREE(pStream->pszBuffer);
			FREE(pStream);
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTCPMultiplexServer Message Handlers
//////////////////////////////////////////////////////////////////////

// CTCPMultiplexServer Daemon Thread Message Handlers -------------------------

BOOL CTCPMultiplexServer::OnEnterDaemon()
{
	return TRUE;
}

void CTCPMultiplexServer::OnLeaveDaemon()
{
}

// CTCPMultiplexServer Session Thread Message Handlers ------------------------

void CTCPMultiplexServer::OnAcceptSession(WORKSESSION *pSession)
{
}

BOOL CTCPMultiplexServer::OnEnterSession(WORKSESSION *pSession)
{
	return TRUE;
}

void CTCPMultiplexServer::OnLeaveSession(WORKSESSION *pSession)
{
}

void CTCPMultiplexServer::OnSendSession(WORKSESSION *pSession, const char* pszBuffer, int nLength)
{
}

BOOL CTCPMultiplexServer::OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength)
{
	return TRUE;
}

void CTCPMultiplexServer::OnErrorSession(WORKSESSION *pSession, int nType)
{
}

void CTCPMultiplexServer::OnCloseSession(WORKSESSION *pSession)
{
}

BOOL CTCPMultiplexServer::OnTimeoutSession(WORKSESSION *pSession)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTCPMultiplexServer Work Thread
//////////////////////////////////////////////////////////////////////

void *CTCPMultiplexServer::WatchThread(void *pParam)
{
	CTCPMultiplexServer	*pThis;

	pThis = (CTCPMultiplexServer *)pParam;
	if (pThis->EnterDaemon())
		pThis->RunDaemon();
	pThis->LeaveDaemon();

	return 0;
}

void *CTCPMultiplexServer::SendThread(void *pParam)
{
	CTCPMultiplexServer	*pThis;

	pThis	 = (CTCPMultiplexServer *)pParam;
	pThis->SessionSender();

	return 0;
}
