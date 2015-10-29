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
#include "CommonUtil.h"
#include "DebugUtil.h"

extern BOOL	m_bExitSignalPending;

//////////////////////////////////////////////////////////////////////
// CMobileClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define STATE_WHITE			0
#define STATE_HEADER		1
#define STATE_DATA			2

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

BOOL CMobileClient::Initialize()
{
	m_pQueue = new CQueue(4096);
	CSerialServer::DisableSendFail(TRUE);
#if 0	
	return CSerialServer::Startup("/dev/ttyS00", 1, 5*60, B9600, NONE_PARITY, HUPCL | CRTSCTS);
#else
	return CSerialServer::Startup("/dev/ttyO1", 1, 5*60, B115200, NONE_PARITY, HUPCL ); //2015-10-27 sungyeung : ? is this available?
#endif
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

	if (nLength == -1) nLength = strlen(pszBuffer);
	return CSerialServer::DirectSendToSession(m_pSaveSession, pszBuffer, nLength);
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
	TIMETICK start, now;
	int		nElapse, c, nBytes = 0;

	GetTimeTick(&start);
	for(;;)
	{
		c = m_pQueue->GetQ();
		if (c == -1)
		{
			GetTimeTick(&now);
			nElapse = GetTimeInterval(&start, &now);
			if (nElapse > nTimeout)
			{
				if (nBytes == 0)
					return -1;
				pszBuffer[nBytes] = 0;
				break;
			}

			USLEEP(100000);
			continue;
		}

		if ((c == 0xd) || (c == 0xa))
		{
			pszBuffer[nBytes] = 0;
			if (nBytes > 0) break;
			continue;
		}

		pszBuffer[nBytes] = c;
		nBytes++;
		if (nBytes >= nLength)
			break;
	} 
	return nBytes;
}

BOOL CMobileClient::ReadPacketFromModem(char *pszBuffer, int *nLength, int nTimeout)
{
	IF4_HEADER	*pHeader;
	int		nElapse, c, n, nBytes = 0;
	int		nState, len=0;

	*nLength = 0;
	nState   = STATE_WHITE;

	for(nElapse=0; nElapse<nTimeout;)
	{
		switch(nState) {
		  case STATE_WHITE :
			   c = m_pQueue->GetQ();
			   if (c == -1)
			   {
				   usleep(100000);
				   nElapse += 100;
				   break;
			   }

		       if (c != IF4_SOH)
				   break;

  		       pszBuffer[nBytes] = c;
		       nBytes++;
			   nState = STATE_HEADER;
			   break;

		  case STATE_HEADER :
			   n = m_pQueue->GetQ(pszBuffer+nBytes, sizeof(IF4_HEADER)-nBytes);
			   if (n <= 0)
			   {	
				   usleep(100000);
				   nElapse += 100;
				   break;
			   }
			   nBytes += n;
			   if (nBytes < (int)sizeof(IF4_HEADER))
				   break;
			   pHeader = (IF4_HEADER *)pszBuffer;
			   len = sizeof(IF4_HEADER) + sizeof(IF4_TAIL) + pHeader->len;
			   nState = STATE_DATA;
			   break;

		  case STATE_DATA :
			   n = m_pQueue->GetQ(pszBuffer+nBytes, len-nBytes);
			   if (n <= 0)
			   {	
				   usleep(100000);
				   nElapse += 100;
				   break;
			   }
			   nBytes += n;
			   if (nBytes < len)
				   break;

			   *nLength = nBytes;
			   return TRUE;
		}
	} 
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CMobileClient::OnReceiveSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
//	XDEBUG("MobileClient: %d Bytes recv.\r\n", nLength);
//	XDUMP(pszBuffer, nLength, TRUE);

	m_pQueue->AddQ(pszBuffer, nLength);
	return TRUE;
}

void CMobileClient::OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
//	XDEBUG("MobileClient: %d Bytes send.\r\n", nLength);
//	XDUMP(pszBuffer, nLength, TRUE);
}

