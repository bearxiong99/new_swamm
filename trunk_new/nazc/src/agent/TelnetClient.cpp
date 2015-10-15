#include "common.h"
#include "TelnetClient.h"
#include "ZigbeeServer.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "codiapi.h"
#include "if4frame.h"

//////////////////////////////////////////////////////////////////////
// CTelnetClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTelnetClient::CTelnetClient()
{
	m_pSensor 		= NULL;
	m_nTargetPort	= 0;
}

CTelnetClient::~CTelnetClient()
{
}

//////////////////////////////////////////////////////////////////////
// CTelnetClient Operations
//////////////////////////////////////////////////////////////////////

BOOL CTelnetClient::Initialize(void *pSensor, int nPort)
{
	m_pSensor 		= pSensor;
	m_nTargetPort	= nPort;

	time(&m_nLastTryTime);
	return TRUE;
}

void CTelnetClient::Destroy()
{
	if (CTCPClient::IsConnected())
		CTCPClient::Disconnect();
	m_pSensor = NULL;
}

BOOL CTelnetClient::NewConnection()
{
	gettimeofday((struct timeval *)&m_tmConnect, NULL);
	time(&m_nLastTryTime);

	XDEBUG("\xd\xa<<TELNET>> ---------- New Connection ----------\xd\xa");
	if (!CTCPClient::Connect("localhost", m_nTargetPort))
	{
//		XDEBUG("<<TELNET>> Not Connected.\xd\xa");
		CTCPClient::Disconnect();
		return TRUE;
	}

//	XDEBUG("<<TELNET>> TELNET Server '%s' Connected.\xd\xa", GetAddress());
	return TRUE;
}

BOOL CTelnetClient::SendToHost(char *pszBuffer, int nLength, BYTE nAction)
{
	if (!CTCPClient::IsConnected())
	{
		XDEBUG("\r\n<<TELNET>> Not connected.\r\n");
		return FALSE;
	}

	CTCPClient::AddSendStream(pszBuffer, nLength);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTelnetClient Override Functions
//////////////////////////////////////////////////////////////////////

void CTelnetClient::OnConnect()
{
    struct timeval      now;

	gettimeofday((struct timeval *)&now, NULL);
//	XDEBUG("<<TELNET>> %s:%0d Connected (Time=%0d ms)\xd\xa", GetAddress(), GetPort(), GetTimeInterval(&m_tmConnect, &now));
}

void CTelnetClient::OnDisconnect()
{
    TIMETICK      now;

	GetTimeTick(&now);
	XDEBUG("<<TELNET>> %s:%0d (Time=%0d ms)\xd\xa",
			GetAddress(), GetPort(), GetTimeInterval(&m_tmConnect, &now));
	XDEBUG("<<TELNET>> ---------- End ----------\xd\xa\xd\xa");

	m_pSensor = NULL;
}

void CTelnetClient::OnConnectError()
{
	XDEBUG("<<TELNET>> %s:%0d Connect Fail!!\xd\xa", GetAddress(), GetPort());
}

void CTelnetClient::OnSend(LPSTR pszBuffer, int nLength)
{
	time(&m_nLastTryTime);
	XDEBUG("<<TELNET>> SEND %0d Byte(s)\xd\xa", nLength);
	XDUMP(pszBuffer, nLength);
}

BOOL CTelnetClient::OnReceive(LPSTR pszBuffer, int nLength)
{
	ZBSESSION	*pSession;
	CZigbeeServer *pServer = NULL;
    TIMETICK	now;
    IF4_PACKET * packet;

	time(&m_nLastTryTime);
	GetTimeTick(&now);

	XDEBUG("<<TELNET>> RECV %0d Bytes (Time=%0d ms)\xd\xa", nLength, GetTimeInterval(&m_tmConnect, &now));
	//XDUMP(pszBuffer, nLength);

	pSession = (ZBSESSION *)m_pSensor;
	if (pSession == NULL)
		return TRUE;

    pServer = (CZigbeeServer *)pSession->pServer;
    if (pServer == NULL)
        return TRUE;

    /** @todo ZigBee Server에서 SendResult 하는 부분을 EndDevice에 대한 Queue로 만들어야 한다
      * 현재와 같이 계속 retry 하는 것은 위험하다.
      */
#if 0
    while(!pServer->SendResult(pSession, (pszBuffer), nLength))
    {
        usleep(1000000);
        if(cnt > 6) break;
        cnt++;
    }
#else
    packet = (IF4_PACKET *) pszBuffer;
    if(packet->hdr.attr & IF4_ATTR_CTRLFRAME)
    {
        /** Control Frame은 전송하지 않는다 */
        XDEBUG("SendResult SKIP : IF4 Control Frame\r\n");
        return TRUE;
    }

    pServer->SendResult(pSession, pszBuffer, nLength);
#endif
	return TRUE;
}

