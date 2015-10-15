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

#include "common.h"
#include "MobileClient.h"
#include "DebugUtil.h"

extern BOOL	m_bExitSignalPending;

extern int m_nBaudRate;
extern char	m_szTtyName[];

//////////////////////////////////////////////////////////////////////
// CMobileClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileClient	*m_pMobileClient = NULL;

//////////////////////////////////////////////////////////////////////
// CMobileClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileClient::CMobileClient()
{
	m_pMobileClient = this;
	m_pQueue 		= NULL;
}

CMobileClient::~CMobileClient()
{
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CMobileClient::Initialize()
{
    int baud = B9600;

    switch(m_nBaudRate)
    {
        case 115200:
            baud = B115200;
            break;
    }
	m_pQueue = new CQueue(4096);
	CSerialServer::DisableSendFail(TRUE);
	return CSerialServer::Startup(m_szTtyName, 1, 5*60, baud, NONE_PARITY, HUPCL | CRTSCTS);
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

	if (m_pSaveSession == NULL)
		return -1;

	szBuffer[0] = c;
	szBuffer[1] = 0;	
	return CSerialServer::DirectSendToSession(m_pSaveSession, szBuffer, 1);
}

int CMobileClient::WriteToModem(char *pszBuffer, int nLength)
{
	if (m_pSaveSession == NULL)
		return -1;

	// mnet에서는 반드시 시리얼에 Direct로 써야 한다.
	// ZMODEM이 정상적으로 동작하지 않을 수 있다.
	if (nLength == -1) nLength = strlen(pszBuffer);
	return CSerialServer::DirectSendToSession(m_pSaveSession, pszBuffer, nLength);
}

int CMobileClient::ReadFromModem(char *pszBuffer, int nLength, int nTimeout)
{
	int		nElapse, n, nBytes = 0;

	for(nElapse=0; nElapse<nTimeout; nElapse+=10)
	{
		n = m_pQueue->GetQ(pszBuffer+nBytes, nLength-nBytes);
		if (n > 0)
		{
			nBytes += n;
			if (nBytes >= nLength)
				break;
		}
		usleep(10000);
	} 
	return nBytes;
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CMobileClient::OnReceiveSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
	m_pQueue->AddQ(pszBuffer, nLength);
	return TRUE;
}

void CMobileClient::OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
//	printf("MobileClient: %d Bytes send.\r\n", nLength);
//	DUMPSTRING(pszBuffer, nLength);
}

