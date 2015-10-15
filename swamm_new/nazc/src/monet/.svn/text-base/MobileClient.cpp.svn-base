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
	m_bDisplay 		= FALSE;
	m_nLength		= 0;
}

CMobileClient::~CMobileClient()
{
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Operations
//////////////////////////////////////////////////////////////////////

void CMobileClient::SetDisplay(BOOL bDisplay)
{
	m_bDisplay = bDisplay;
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CMobileClient::Initialize(char *pszDevice, int nSpeed)
{
	m_pQueue = new CQueue(4096);
	CSerialServer::DisableSendFail(TRUE);
	return CSerialServer::Startup(pszDevice, 1, 5*60, nSpeed, NONE_PARITY, HUPCL | CRTSCTS);
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
	int		i;
	BYTE	c;

//	printf("MobileClient: %d Bytes recv.\r\n", nLength);
//	DUMPSTRING(pszBuffer, nLength);

	if (!m_bDisplay)
	{
		 m_pQueue->AddQ(pszBuffer, nLength);
	}
	else
	{
		for(i=0; i<nLength; i++)
		{
			c = (BYTE)pszBuffer[i];
			switch(c) {
			  case 0xa :
				   m_szLine[m_nLength] = 0;
				   if (strstr(m_szLine, "NURI/1.0 212") != NULL)
					   m_bExitSignalPending = TRUE;
				   break;
			  case 0xd :
				   m_szLine[m_nLength] = 0;
				   if (strstr(m_szLine, "NO CARRIER") != NULL)
					   m_bExitSignalPending = TRUE;
				   m_nLength = 0;
				   break;
			  default :
				   m_szLine[m_nLength] = c;
				   m_nLength++;
				   break;
			}
		}		
		write(1, pszBuffer, nLength);
	}
	return TRUE;
}

void CMobileClient::OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
//	printf("MobileClient: %d Bytes send.\r\n", nLength);
//	DUMPSTRING(pszBuffer, nLength);
}

