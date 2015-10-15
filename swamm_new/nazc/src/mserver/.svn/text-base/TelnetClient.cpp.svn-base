#include "common.h"
#include "TelnetClient.h"
#include "MobileServer.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CTelnetClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTelnetClient	*m_pTelnetClient = NULL;
extern	BOOL    m_bExitSignalPending;

//////////////////////////////////////////////////////////////////////
// CTelnetClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTelnetClient::CTelnetClient()
{
	m_pTelnetClient = this;
	m_bReceived = FALSE;
}

CTelnetClient::~CTelnetClient()
{
}

//////////////////////////////////////////////////////////////////////
// CTelnetClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CTelnetClient::IsReceived()
{
	return m_bReceived;
}

//////////////////////////////////////////////////////////////////////
// CTelnetClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CTelnetClient::NewConnection(int nPort)
{
//	printf("\xd\xa---------- New Connection ----------\xd\xa");
	gettimeofday((struct timeval *)&m_tmConnect, NULL);
	time(&m_nLastTryTime);

	m_bReceived = FALSE;
	if (!CTCPClient::Connect("localhost", nPort))
	{
//		XDEBUG("Not Connected.\xd\xa");
		CTCPClient::Disconnect();
		return TRUE;
	}

//	printf("Connect Server '%s' Connected.\xd\xa", GetAddress());
	return TRUE;
}

void CTelnetClient::DeleteConnection()
{
	if (CTCPClient::IsConnected())
		CTCPClient::Disconnect();
}

BOOL CTelnetClient::SendToHost(char *pszBuffer, int nLength)
{
	if (!CTCPClient::IsConnected())
		return FALSE;

	if (m_bExitSignalPending)
		return TRUE;

	CTCPClient::AddSendStream(pszBuffer, nLength);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTelnetClient Override Functions
//////////////////////////////////////////////////////////////////////

void CTelnetClient::OnConnect()
{
	time(&m_nLastTryTime);
//	printf("%s:%0d Connected\xd\xa", GetAddress(), GetPort());
}

void CTelnetClient::OnDisconnect()
{
//	printf("%s:%0d Disconnected\xd\xa", GetAddress(), GetPort());
//	printf("---------- End ----------\xd\xa");
	m_pMobileServer->TerminateAlert();
}

void CTelnetClient::OnConnectError()
{
//	printf("%s:%0d Connect Fail!!\xd\xa", GetAddress(), GetPort());
}

void CTelnetClient::OnSend(LPSTR pszBuffer, int nLength)
{
//	printf("SEND %0d Byte(s)\xd\xa", nLength);
//	XDUMP(pszBuffer, nLength);
}

BOOL CTelnetClient::OnReceive(LPSTR pszBuffer, int nLength)
{
	time(&m_nLastTryTime);
	m_bReceived = TRUE;
//	printf("RECV %0d Bytes\xd\xa", nLength);
//	DUMPSTRING(pszBuffer, nLength);
	m_pMobileServer->WriteToModem(pszBuffer, nLength);
	return TRUE;
}

