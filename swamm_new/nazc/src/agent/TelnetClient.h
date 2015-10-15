#ifndef __TELNET_CLIENT_H__
#define __TELNET_CLIENT_H__

#include "TCPClient.h"
#include "Locker.h"

class CTelnetClient : public CTCPClient
{
public:
	CTelnetClient();
	virtual ~CTelnetClient();

public:
	virtual BOOL Initialize(void *pSensor, int nPort);
	virtual void Destroy();

	BOOL	NewConnection();
	BOOL	SendToHost(char *pszBuffer, int nLength, BYTE nAction);

protected:
	BOOL 	OnActiveThread();
	BOOL 	OnTimeoutThread();

    void	OnConnect();
    void	OnDisconnect();
    void	OnConnectError();
    void	OnSend(LPSTR pszBuffer, int nLength);
    BOOL	OnReceive(LPSTR pszBuffer, int nLength);

private:
	int					m_nTargetPort;
	void				*m_pSensor;
	time_t				m_nLastTryTime;
	TIMETICK        	m_tmConnect;
};

#endif	// __TELENT_CLIENT_H__
