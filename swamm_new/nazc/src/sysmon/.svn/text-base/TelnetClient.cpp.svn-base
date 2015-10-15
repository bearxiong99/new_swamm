#include "common.h"
#include "TelnetClient.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CTelnetClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTelnetClient::CTelnetClient()
{
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
	gettimeofday((struct timeval *)&m_tmConnect, NULL);
	time(&m_nLastTryTime);

	m_bReceived = FALSE;
	if (!CTCPClient::Connect("localhost", nPort))
	{
		CTCPClient::Disconnect();
		return TRUE;
	}
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

	CTCPClient::AddSendStream(pszBuffer, nLength);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTelnetClient Override Functions
//////////////////////////////////////////////////////////////////////

void CTelnetClient::OnConnect()
{
	time(&m_nLastTryTime);
}

void CTelnetClient::OnDisconnect()
{
}

void CTelnetClient::OnConnectError()
{
}

void CTelnetClient::OnSend(LPSTR pszBuffer, int nLength)
{
}

BOOL CTelnetClient::OnReceive(LPSTR pszBuffer, int nLength)
{
	time(&m_nLastTryTime);
	m_bReceived = TRUE;
	return TRUE;
}

