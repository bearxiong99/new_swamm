#ifndef __TELNET_CLIENT_H__
#define __TELNET_CLIENT_H__

#include "TCPClient.h"

class CTelnetClient : public CTCPClient
{
public:
	CTelnetClient();
	virtual ~CTelnetClient();

public:
	BOOL	IsReceived();

public:
	BOOL	NewConnection(int nPort);
	void	DeleteConnection();
	BOOL	SendToHost(char *pszBuffer, int nLength);

protected:
    void	OnConnect();
    void	OnDisconnect();
    void	OnConnectError();
    void	OnSend(LPSTR pszBuffer, int nLength);
    BOOL	OnReceive(LPSTR pszBuffer, int nLength);

private:
	time_t				m_nLastTryTime;
	struct timeval  	m_tmConnect;
	BOOL				m_bReceived;
};

#endif	// __TELENT_CLIENT_H__
