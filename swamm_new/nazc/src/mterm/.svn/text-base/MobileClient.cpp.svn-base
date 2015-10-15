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
	m_nColumn		= 0;
	m_nMode			= DISPLAY_MODE_ASCII;
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

BOOL CMobileClient::Initialize(char *pszDevice, int nSpeed, int nOption, int nDisplayMode)
{
	m_nMode = nDisplayMode;
	m_pQueue = new CQueue(4096);
	CSerialServer::DisableSendFail(TRUE);

//	return CSerialServer::Startup("/dev/ttyS00", 1, 5*60, B115200, NONE_PARITY, HUPCL | CRTSCTS);
	return CSerialServer::Startup(pszDevice, 1, 5*60, nSpeed, NONE_PARITY, nOption);
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
	char	buffer[128];
	char	temp[5];
	int		i;
	BYTE	c;

	// ASCII mode
	if (m_nMode == DISPLAY_MODE_ASCII)
	{
		for(i=0; i<nLength; i++)
		{
			c = pszBuffer[i];
			switch(c) {
			  case 0x0A :
				   write(0, "\n", 1);
				   break;
			  case 0x0D :
				   break;
			    default :
				   write(0, &pszBuffer[i], 1);
				   break;
			}
		}
		fflush(0);
		return TRUE;
	}

	// Binary mode
	*buffer = '\0';
	for(i=0; i<nLength; i++)
	{
		if (m_nColumn == 0)
			 sprintf(temp, "%02X", pszBuffer[i] & 0xff);
		else sprintf(temp, " %02X", pszBuffer[i] & 0xff);
		strcat(buffer, temp);
		m_nColumn++;

		if (m_nColumn >= 25)
		{
			write(0, buffer, strlen(buffer));
			write(0, "\n", 1);
			m_nColumn = 0;
			*buffer = '\0';
		}
	}

	if (*buffer)
		write(0, buffer, strlen(buffer));

	fflush(0);
	return TRUE;
}

void CMobileClient::OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
}

