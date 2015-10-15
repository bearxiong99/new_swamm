#include "common.h"
#include "PingClient.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "Event.h"
#include "if4api.h"
#include "if4frame.h"

//////////////////////////////////////////////////////////////////////
// CPingClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPingClient::CPingClient()
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	
	m_bReplied = FALSE;
}

CPingClient::~CPingClient()
{
	if (m_hEvent != NULL)
		CloseHandle(m_hEvent);
}

//////////////////////////////////////////////////////////////////////
// CPingClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CPingClient::Ping(char *pszAddress, int nPort, int nTimeout, int nRetry)
{
	int		i;

	// 최소 1회 이상 시도한다.
	nRetry = MAX(1, nRetry);

	for(i=0; i<nRetry; i++)
	{
		m_bReplied = FALSE;
		ResetEvent(m_hEvent);

		XDEBUG("TRY CONNECT %s:%d\r\n", pszAddress, nPort);
		if (!CTCPClient::Connect(pszAddress, nPort))
		{
			CTCPClient::Disconnect();
		}
		else
		{
			WaitForSingleObject(m_hEvent, nTimeout);
			CTCPClient::Disconnect();
			XDEBUG("CONNECT = %d\r\n", m_bReplied);
			if (m_bReplied)
				return TRUE;
		}

		// 1초 쉬고 다시 진행한다.
		usleep(1000000);
	}
	XDEBUG("NOT CONNECT\r\n");
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPingClient Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CPingClient::OnReceive(LPSTR pszBuffer, int nLength)
{
	IF4_CTRL_FRAME	*pFrame;

	XDEBUG("RECV: %d byte(s)\r\n", nLength);
	XDUMP(pszBuffer, nLength, TRUE);

	pFrame = (IF4_CTRL_FRAME *)pszBuffer;
	if ((pFrame->hdr.soh == IF4_SOH) &&
		(pFrame->hdr.attr & IF4_ATTR_CTRLFRAME) &&
		(pFrame->hdr.svc == IF4_SVC_CONTROL) &&
		(pFrame->code == IF4_ENQ))
	{
		m_bReplied = TRUE;
		SetEvent(m_hEvent);
	}
	return TRUE;
}

