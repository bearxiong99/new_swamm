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
#include "SmsWorker.h"
#include "DebugUtil.h"
#include "Variable.h"

#if defined(__TI_AM335X__)
//CMobileClient   *m_pMobileClient = NULL;
#endif

extern MOBILECFGENTRY m_sMobileConfig;

//////////////////////////////////////////////////////////////////////
// CMobileClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileClient::CMobileClient()
{
#if defined(__TI_AM335X__)
//    m_pMobileClient = this;
#endif
	m_pQueue = new CQueue(4096);
	m_bDisplay 	= FALSE;
	m_nLength	= 0;
}

CMobileClient::~CMobileClient()
{
    Destroy();
	if (m_pQueue != NULL)
		delete m_pQueue;
	m_pQueue = NULL;
    fprintf(stderr,"REWRITER delete Mobile client\n");
}

//////////////////////////////////////////////////////////////////////
// CMobileClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CMobileClient::Initialize()
{
	int		nSpeed;

    if(IsStarted()) 
    {
        m_pQueue->FlushQ();
        return TRUE;
    }

	CSerialServer::DisableSendFail(TRUE);

    nSpeed = B115200;
    switch(m_sMobileConfig.interfaceSpeed)
    {
        case 9600: nSpeed = B9600; break;
        case 19200: nSpeed = B19200; break;
        case 38400: nSpeed = B38400; break;
        case 115200: nSpeed = B115200; break;
        case 500000: nSpeed = B500000; break;
    }

	BOOL bRes = CSerialServer::Startup(m_sMobileConfig.interface, 1, 5*60, nSpeed, NONE_PARITY,  0);

    //DisableEcho();
    if(m_sMobileConfig.moduleFlag & MOBILE_MODULE_FLAG_PPP_CTRL_MASK)
    {
        EnableFlowControl(TRUE);
    }

    return bRes;
}

void CMobileClient::Destroy()
{
	CSerialServer::DisableSendFail(FALSE);
	CSerialServer::Shutdown();
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
#if 0
	if (nBytes > 1)
		XDEBUG("%s\r\n", pszBuffer);
#endif
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
	//XDEBUG("MOBILE: Recv %d bytes\r\n", nLength);
	//XDUMP(pszBuffer, nLength, TRUE);

	m_pQueue->AddQ(pszBuffer, nLength);

#if defined(__TI_AM335X__)
    // SMS Message Handler
    //m_pSmsWorker->AddMessage(pszBuffer, nLength);
#endif

	return TRUE;
}

void CMobileClient::OnSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength)
{
	//XDEBUG("MOBILE: Send %d bytes\r\n", nLength);
	//XDUMP(pszBuffer, nLength, TRUE);
}

