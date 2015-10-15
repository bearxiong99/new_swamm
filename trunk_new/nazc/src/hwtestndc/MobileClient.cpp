//////////////////////////////////////////////////////////////////////
//
//	MobileClient.cpp: implementation of the CMobileClient class.
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
#include <string.h>
#include <unistd.h>

#include "MobileClient.h"
#include "DebugUtil.h"

CMobileClient   *m_pMobileClient = NULL;

//////////////////////////////////////////////////////////////////////
// CMobileClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileClient::CMobileClient()
{
    m_pMobileClient = this;
	m_pQueue = new CQueue(4096);
	m_bDisplay 	= FALSE;
	m_nLength	= 0;
}

CMobileClient::~CMobileClient()
{
    Destroy();
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CMobileClient::Initialize(char *interface)
{
	int		nSpeed;

    if(IsStarted()) 
    {
        m_pQueue->FlushQ();
        return TRUE;
    }

	CSerialServer::DisableSendFail(TRUE);

    nSpeed = B115200;

	BOOL bRes = CSerialServer::Startup(interface, 1, 5*60, nSpeed, NONE_PARITY,  0);
    EnableFlowControl(TRUE);

    return bRes;
}

void CMobileClient::Destroy()
{
	CSerialServer::DisableSendFail(FALSE);
	CSerialServer::Shutdown();

	if (m_pQueue != NULL)
		delete m_pQueue;
	m_pQueue = NULL;
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Override Functions
//////////////////////////////////////////////////////////////////////

int CMobileClient::WriteToModem(int c)
{
	char	szBuffer[2];
    int     nSend;

	if (m_pSaveSession == NULL)
		return -1;

	szBuffer[0] = c;
	szBuffer[1] = 0;	
	if((nSend=CSerialServer::DirectSendToSession(m_pSaveSession, szBuffer, 1))<=0)
    {
        CSerialServer::Shutdown();
    }
    return nSend;
}

int CMobileClient::WriteToModem(const char *pszBuffer, int nLength)
{
    int     nSend;

	if (m_pSaveSession == NULL)
		return -1;

	// mnet에서는 반드시 시리얼에 Direct로 써야 한다.
	// ZMODEM이 정상적으로 동작하지 않을 수 있다.
	if (nLength == -1) nLength = strlen(pszBuffer);
    if (nLength <= 0) return 0;

	if((nSend=CSerialServer::DirectSendToSession(m_pSaveSession, pszBuffer, nLength))<=0)
    {
        CSerialServer::Shutdown();
    }
    return nSend;
}

int CMobileClient::ReadFromModem(char *pszBuffer, int nLength, int nTimeout)
{
	int		nElapse, n, nBytes = 0;

	for(nElapse=0; nElapse<nTimeout; nElapse+=100)
	{
		n = m_pQueue->GetQ(pszBuffer+nBytes, nLength-nBytes);
		if (n > 0)
		{
			nBytes += n;
			if (nBytes >= nLength)
				break;
		}
		usleep(100000);
	} 
	return nBytes;
}

int CMobileClient::ReadLineFromModem(char *pszBuffer, int nLength, int nTimeout)
{
	int		nElapse, c, nBytes = 0;

	for(nElapse=0; nElapse<nTimeout;)
	{
		c = m_pQueue->GetQ();
		if (c == -1)
		{
			usleep(100000);
		    nElapse += 100;
			continue;
		}

		if ((c == 0xd) || (c == 0xa))
		{
			pszBuffer[nBytes] = 0;
		    nBytes++;
			break;
		}

		pszBuffer[nBytes] = c;
		nBytes++;
		if (nBytes >= nLength)
			break;
	}
	return nBytes;
}

void CMobileClient::Flush()
{
	m_pQueue->FlushQ();
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CMobileClient::OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength)
{
	m_pQueue->AddQ(pszBuffer, nLength);

	return TRUE;
}

void CMobileClient::OnSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength)
{
}

